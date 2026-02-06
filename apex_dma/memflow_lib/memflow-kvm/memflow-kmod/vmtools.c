// SPDX-License-Identifier: GPL-2.0
#include "vmtools.h"
#include "main.h"
#include "kallsyms/ksyms.h"
#include "../mabi.h"
#include <linux/kvm_host.h>
#include <linux/anon_inodes.h>
#include <linux/sort.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/version.h>
#include "mmap_lock.h"

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,17,0)
#define kvm_for_each_memslot2(memslot, bkt, slots) (void)bkt; kvm_for_each_memslot(memslot, slots)
#else
#define kvm_for_each_memslot2 kvm_for_each_memslot
#endif

static int memflow_vm_release(struct inode *inode, struct file *filp);
static long memflow_vm_ioctl(struct file *filp, unsigned int cmd, unsigned long argp);

static const struct file_operations memflow_vm_fops = {
	.release = memflow_vm_release,
	.unlocked_ioctl = memflow_vm_ioctl,
	.owner = THIS_MODULE
};

static int memflow_vm_mapped_release(struct inode *inode, struct file *filp);

struct vm_vma_map {
	unsigned long start, end;
};

struct vm_mapped_data {
	u32 mapped_vma_count;
	struct vm_vma_map vma_maps[KVM_MEM_SLOTS_NUM];
	vm_map_info_t vm_map_info;
	struct vm_memslot map_slots[KVM_MEM_SLOTS_NUM];
};

static const struct file_operations memflow_vm_mapped_fops = {
	.release = memflow_vm_mapped_release,
	.owner = THIS_MODULE
};

KSYMDEC(kvm_lock);
KSYMDEC(vm_list);

static int open_kvm(struct kvm *kvm)
{
	int fd = -1;

	fd = anon_inode_getfd("memflow-vm", &memflow_vm_fops, kvm, O_RDWR | O_CLOEXEC);

	if (fd < 0) {
		goto fail_fd;
	}

	return fd;

fail_fd:
	kvm_put_kvm(kvm);

	return -1;
}

int open_vm(pid_t target_pid)
{
	struct kvm *kvm;
	int ret = -1;

	mutex_lock(_kvm_lock);

	list_for_each_entry(kvm, _vm_list, vm_list) {
		kvm_get_kvm(kvm);

		if (target_pid == 0 || target_pid == kvm->userspace_pid) {
			ret = open_kvm(kvm);
			break;
		}

		kvm_put_kvm(kvm);
	}

	mutex_unlock(_kvm_lock);

	return ret;
}

static int memflow_vm_release(struct inode *inode, struct file *filp)
{
	struct kvm *kvm = filp->private_data;
	kvm_put_kvm(kvm);
	return 0;
}

static int memslot_compare(const void *lhs, const void *rhs) {
	gfn_t lbase = ((vm_memslot_t *)lhs)->base;
	gfn_t rbase = ((vm_memslot_t *)rhs)->base;

	if (lbase < rbase)
		return -1;
	if (lbase > rbase)
		return 1;
	return 0;
}

static int get_sorted_memslots(struct kvm_memslots *slots, int max_slots, vm_memslot_t *slots_out)
{
	struct kvm_memory_slot *slot;
	int slot_count, bkt;

	slot_count = 0;
	kvm_for_each_memslot2(slot, bkt, slots) {
		if (slot->npages && slot->npages != -1) {
			slots_out[slot_count++] = (vm_memslot_t) {
				.base = gfn_to_gpa(slot->base_gfn),
				.host_base = slot->userspace_addr,
				.map_size = gfn_to_gpa(slot->npages)
			};
		}
	}

	sort(slots_out, slot_count, sizeof(*slots_out), memslot_compare, NULL);

	//TODO: coalesce nearby slots

	return slot_count;
}

static int get_vm_info(struct kvm *kvm, vm_info_t __user *user_info)
{
	vm_info_t kernel_info;
	vm_memslot_t *memslot_map;
	vm_memslot_t __user *user_slots;
	struct kvm_memslots *slots;
	struct kvm_memory_slot *slot;
	int ret, bkt;
	u32 used_slots, slot_count;

	ret = -1;

	if (copy_from_user(&kernel_info, user_info, sizeof(vm_info_t)))
		goto do_return;

	user_slots = kernel_info.slots;

	mutex_lock(&kvm->lock);
	mutex_lock(&kvm->slots_lock);

	if (!kernel_info.slot_count)
		goto unlock_kvm;

	// Clamp user provided sizes...
	slots = kvm_memslots(kvm);
	used_slots = 0;
	kvm_for_each_memslot2(slot, bkt, slots) {
		used_slots++;
	}
	if (kernel_info.slot_count > used_slots)
		kernel_info.slot_count = used_slots;

	// Supposedly exactly used_slots should have non-zero sized values, but what if not?
	memslot_map = vmalloc(sizeof(*memslot_map) * kernel_info.slot_count);

	if (!memslot_map)
		goto unlock_kvm;

	if ((slot_count = get_sorted_memslots(kvm_memslots(kvm), kernel_info.slot_count, memslot_map)) == -1)
		goto free_slots;

	kernel_info.userspace_pid = kvm->userspace_pid;
	kernel_info.slot_count = slot_count;

	if (copy_to_user(user_info, &kernel_info, sizeof(vm_info_t)))
		goto free_slots;
	if (slot_count && copy_to_user(user_slots, memslot_map, sizeof(*memslot_map) * slot_count))
		goto free_slots;

	ret = 0;

free_slots:
	vfree(memslot_map);
unlock_kvm:
	mutex_unlock(&kvm->slots_lock);
	mutex_unlock(&kvm->lock);
do_return:
	return ret;
}

struct vm_mem_data {
	// This field is only valid pre-mmap call.
	struct vm_area_struct *wrapped_vma;
	struct task_struct *wrapped_task;
	int nr_pages;
	struct page **pages;
};

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0) && LINUX_VERSION_CODE < KERNEL_VERSION(5,10,0)
#define PAGE_GET_FLAG FOLL_LONGTERM
#else
#define PAGE_GET_FLAG 0
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)
#define RELEASE_PAGE put_page
#else
#define PAGE_GET_FLAG 0
#define RELEASE_PAGE put_user_page
#endif

static int memflow_vm_mem_release(struct inode *inode, struct file *file)
{
	struct vm_mem_data *data = file->private_data;
	int i;

	if (data) {
		if (data->pages) {
			for (i = 0; i < data->nr_pages; i++)
				RELEASE_PAGE(data->pages[i]);
			vfree(data->pages);
		}

		vfree(data);
	}

	return 0;
}

static int memflow_vm_mem_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct vm_mem_data *data = file->private_data;
	unsigned long nr_pages = (vma->vm_end - vma->vm_start) >> PAGE_SHIFT;
	int i, ret = -1;
	unsigned long foll_flags = PAGE_GET_FLAG|FOLL_GET;
	pgprot_t remap_flags = PAGE_SHARED;
#if LINUX_VERSION_CODE < KERNEL_VERSION(6,5,0)
	struct vm_area_struct **tmp_vmas;
#endif

	if (vma->vm_flags & VM_WRITE)
		foll_flags |= FOLL_WRITE;
	else
		remap_flags = PAGE_READONLY;

	if (data->wrapped_vma->vm_end - data->wrapped_vma->vm_start != nr_pages << PAGE_SHIFT)
		goto do_return;

	data->pages = vmalloc(sizeof(*data->pages) * nr_pages);

	if (!data->pages)
		goto do_return;

#if LINUX_VERSION_CODE < KERNEL_VERSION(6,5,0)
	tmp_vmas = vmalloc(sizeof(*tmp_vmas) * nr_pages);
	if (!tmp_vmas)
		goto free_pages;
#endif

	data->nr_pages = get_user_pages_remote(
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,9,0)
		data->wrapped_task,
#endif
		data->wrapped_task->mm,
		data->wrapped_vma->vm_start,
		nr_pages,
		foll_flags,
		data->pages,
#if LINUX_VERSION_CODE < KERNEL_VERSION(6,5,0)
		tmp_vmas,
#endif
		NULL
	);

#if LINUX_VERSION_CODE < KERNEL_VERSION(6,5,0)
	// We really don't need them, but we vmalloc it, because the kernel kcallocs it, and it fails?
	vfree(tmp_vmas);
#endif

	if (data->nr_pages != nr_pages)
		goto put_pages;

#if LINUX_VERSION_CODE < KERNEL_VERSION(6,3,0)
	vma->vm_flags |= VM_PFNMAP | VM_DONTDUMP;
#else
	vm_flags_set(vma, VM_PFNMAP | VM_DONTDUMP);
#endif

	ret = 0;

	// We would normally use vm_insert_pages, but the given pages may be compound
	for (i = 0; i < data->nr_pages; i++) {
		ret = remap_pfn_range(vma, vma->vm_start + i * (1ul << PAGE_SHIFT), page_to_pfn(data->pages[i]), 1ul << PAGE_SHIFT, remap_flags);
		if (ret) {
			//Unmap all mapped pages
			break;
		}
	}

	if (ret || data->nr_pages != nr_pages)
		goto put_pages;

	data->wrapped_vma = NULL;
	data->wrapped_task = NULL;

	return 0;

put_pages:
	for (i = 0; i < data->nr_pages; i++)
		RELEASE_PAGE(data->pages[i]);
#if LINUX_VERSION_CODE < KERNEL_VERSION(6,5,0)
free_pages:
#endif
	vfree(data->pages);
	data->nr_pages = 0;
	data->pages = NULL;
do_return:
	return ret;
}

static unsigned long memflow_vm_mem_get_unmapped_area(struct file *file, unsigned long addr, unsigned long len, unsigned long pgoff, unsigned long flags)
{
    struct vm_mem_data *data = file->private_data;
#if LINUX_VERSION_CODE < KERNEL_VERSION(6,10,0)
    return get_unmapped_area(data->wrapped_vma->vm_file, addr, len, pgoff + data->wrapped_vma->vm_pgoff, flags);
#else
    return mm_get_unmapped_area(data->wrapped_task->mm, data->wrapped_vma->vm_file, addr, len, pgoff + data->wrapped_vma->vm_pgoff, flags);
#endif
}

static const struct file_operations memflow_vm_mem_fops = {
	.release = memflow_vm_mem_release,
	.mmap = memflow_vm_mem_mmap,
	.get_unmapped_area = memflow_vm_mem_get_unmapped_area,
	.owner = THIS_MODULE
};

static unsigned long mmap_vma(struct vm_area_struct *vma, struct task_struct *task)
{
	struct file *wrap_file;
	unsigned long page_prot = PROT_READ;
	struct vm_mem_data *priv;
	unsigned long ret = -1;

	priv = vmalloc(sizeof(*priv));

	if (!priv)
		goto do_return;

	priv->pages = NULL;
	priv->wrapped_vma = vma;
	priv->wrapped_task = task;

	wrap_file = anon_inode_getfile("memflow-vm-mem", &memflow_vm_mem_fops, priv, O_RDWR);

	if (IS_ERR_OR_NULL(wrap_file))
		goto free_priv;

	if (vma->vm_flags & VM_WRITE)
		page_prot |= PROT_WRITE;

	ret = vm_mmap(wrap_file, 0, vma->vm_end - vma->vm_start, page_prot, MAP_SHARED, 0);

	fput(wrap_file);

	return ret;

free_priv:
	vfree(priv);
do_return:
	return ret;
}

// Called with otask->mm->mm_sem locked for writing. Does not release the lock
static void remap_vmas(struct vm_mapped_data *data, struct task_struct *otask)
{
	int i, o, u;
	vm_memslot_t *slot;
	struct vm_vma_map *mapped_vma;
	struct vm_area_struct *vma;
	unsigned long retaddr;
	unsigned long offset;

	for (i = 0; i < data->mapped_vma_count; i++) {
		mapped_vma = data->vma_maps + i;

		vma = find_vma(otask->mm, mapped_vma->start);

		if (!vma)
			goto remove_vma_map_entry;

		retaddr = mmap_vma(vma, otask);

		if (IS_ERR((void *)retaddr))
			goto remove_unmapped_slots;

		// Update the internal structure to point to the current userspace
		offset = retaddr - mapped_vma->start;

		for (o = 0; o < data->vm_map_info.slot_count; o++) {
			slot = data->vm_map_info.slots + o;
			if (slot->host_base >= mapped_vma->start && slot->host_base < mapped_vma->end) {
				slot->host_base += offset;
			}
		}

		mapped_vma->start += retaddr;
		mapped_vma->end += offset;

		continue;

remove_unmapped_slots:
		// Remove all memslots that correspond to this vma
		for (o = data->vm_map_info.slot_count - 1; o >= 0; o--) {
			slot = data->vm_map_info.slots + o;
			if (slot->host_base >= vma->vm_start && slot->host_base < vma->vm_end) {
				data->vm_map_info.slot_count--;
				for (u = o; u < data->vm_map_info.slot_count; u++) {
					data->vm_map_info.slots[u] = data->vm_map_info.slots[u + 1];
				}
			}
		}

remove_vma_map_entry:
		// Do swap remove
		data->vma_maps[i--] = data->vma_maps[--data->mapped_vma_count];
	}
}

static void find_unique_vmas(struct vm_mapped_data *data, struct mm_struct *other_mm)
{
	int i, o;
	struct vm_area_struct *vma;
	vm_memslot_t *slot;

	for (i = 0; i < data->vm_map_info.slot_count; i++) {
		slot = data->vm_map_info.slots + i;
		vma = find_vma(other_mm, slot->host_base);

		for (o = 0; o < data->mapped_vma_count; o++) {
			if (vma->vm_start == data->vma_maps[o].start)
				goto skip_slot;
		}

		data->vma_maps[data->mapped_vma_count++] = (struct vm_vma_map) {
			.start = vma->vm_start,
			.end = vma->vm_end
		};

skip_slot:
		continue;
	}
}

static int do_map_vm(struct kvm *kvm, vm_map_info_t __user *user_info)
{
	struct vm_mapped_data *priv;
	int fd = -1, memslot_count;
	struct file *file;
	struct task_struct *other_task;
	struct mm_struct *other_mm;
	vm_map_info_t user_info_copied;

	other_task = pid_task(find_vpid(kvm->userspace_pid), PIDTYPE_PID);

	// We could support doing the remapping in current process, but it's pointless and adds extra lock complexity
	if (!other_task || other_task == current)
		goto do_return;

	other_mm = other_task->mm;

	if (!other_mm || other_mm == current->mm)
		goto do_return;

	priv = vmalloc(sizeof(*priv));

	if (!priv)
		goto do_return;

	priv->mapped_vma_count = 0;
	priv->vm_map_info.slot_count = 0;

	if (copy_from_user(&user_info_copied, user_info, sizeof(vm_map_info_t)))
		goto free_alloc;

	priv->vm_map_info = user_info_copied;
	priv->vm_map_info.slots = priv->map_slots;

	if (!priv->vm_map_info.slot_count)
		goto free_alloc;

	if (priv->vm_map_info.slot_count > KVM_MEM_SLOTS_NUM)
		priv->vm_map_info.slot_count = KVM_MEM_SLOTS_NUM;

	fd = get_unused_fd_flags(O_CLOEXEC);

	if (fd < 0)
		goto free_alloc;

	mutex_lock(&kvm->lock);
	mutex_lock(&kvm->slots_lock);

	if ((memslot_count = get_sorted_memslots(kvm_memslots(kvm), priv->vm_map_info.slot_count, priv->vm_map_info.slots)) == -1) {
        mutex_unlock(&kvm->lock);
        mutex_unlock(&kvm->slots_lock);
		goto put_fd;
    }

	priv->vm_map_info.slot_count = memslot_count;

	mmap_write_lock(other_mm);

    // Once we hold mmap_sem, the slots won't be freed so there is no purpose to hold the locks
    mutex_unlock(&kvm->lock);
	mutex_unlock(&kvm->slots_lock);

	// First order of business is to grab all unique mappings to memslots (that are backed by some kind of file)
	find_unique_vmas(priv, other_mm);

	file = anon_inode_getfile("memflow-vm-map", &memflow_vm_mapped_fops, priv, O_RDWR);

	if (IS_ERR_OR_NULL(file))
		goto unlock_mem;

	// Now remap all unique mappings
	remap_vmas(priv, other_task);

	if (!priv->mapped_vma_count)
		goto release_file;

	user_info_copied.slot_count = priv->vm_map_info.slot_count;

	if (copy_to_user(user_info, &user_info_copied, sizeof(vm_map_info_t)))
		goto release_file;
	if (priv->vm_map_info.slot_count && copy_to_user(user_info_copied.slots, priv->vm_map_info.slots, sizeof(vm_memslot_t) * priv->vm_map_info.slot_count))
		goto release_file;

	fd_install(fd, file);

	mmap_write_unlock(other_mm);

	return fd;

release_file:
	// The data will be freed later on, so we do not have to do that ourselves
	priv = NULL;
	fput(file);
unlock_mem:
	mmap_write_unlock(other_mm);
put_fd:
	put_unused_fd(fd);
free_alloc:
	if (priv)
		vfree(priv);
do_return:
	return -1;
}

static long memflow_vm_ioctl(struct file *filp, unsigned int cmd, unsigned long argp)
{
	switch (cmd) {
		case MEMFLOW_VM_INFO:
			return get_vm_info(filp->private_data, (vm_info_t __user *)argp);
		case MEMFLOW_MAP_VM:
			return do_map_vm(filp->private_data, (vm_map_info_t __user *)argp);
	}

	return -1;
}

static int memflow_vm_mapped_release(struct inode *inode, struct file *filp)
{
	vfree(filp->private_data);
	return 0;
}
