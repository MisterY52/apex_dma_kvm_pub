#include <linux/types.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/printk.h>
#include <linux/uaccess.h>
#include <linux/kallsyms.h>
#include "kmem.h"

MODULE_DESCRIPTION("vmread in-kernel helper used to accelerate memory operations");
MODULE_AUTHOR("Heep");
MODULE_LICENSE("GPL");

static int vmreadinit(void);
static void vmreadexit(void);
static void vmmap(ProcessData* data);
static long unlocked_ioctl(struct file* filp, unsigned int cmd, unsigned long argp);

module_init(vmreadinit);
module_exit(vmreadexit);

#define KSYMDEC(x) static typeof(&x) _##x = NULL
#define KSYM(x) _##x
#define KSYMDEF(x) _##x = (typeof(&x))kallsyms_lookup_name(#x)

KSYMDEC(insert_vm_struct);
KSYMDEC(vm_area_alloc);
KSYMDEC(vm_area_free);

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = unlocked_ioctl
};

static int vmreadinit(void)
{
	proc_create("vmread", 0, 0, &fops);
	KSYMDEF(insert_vm_struct);
	KSYMDEF(vm_area_alloc);
	KSYMDEF(vm_area_free);

	printk("vmread: initialized\n");
	return 0;
}

static void vmreadexit(void)
{
	remove_proc_entry("vmread", NULL);
	printk("vmread: uninitialized\n");
}

static long unlocked_ioctl(struct file* filp, unsigned int cmd, unsigned long argp)
{
	void* __user userArgs;
	ProcessData kernelArgs;

	userArgs = (void* __user)argp;

	switch (cmd) {
		case VMREAD_IOCTL_MAPVMMEM:
			if (copy_from_user(&kernelArgs, userArgs, sizeof(kernelArgs)))
				return -EFAULT;
			vmmap(&kernelArgs);
			if (copy_to_user(userArgs, &kernelArgs, sizeof(kernelArgs)))
				return -EFAULT;
			break;
	}

	return 0;
}

static void map_part_of_vm(struct vm_area_struct* vma, uint64_t targetAddr, uint64_t size, uint64_t pfn)
{
	uint64_t end = targetAddr + PAGE_ALIGN(size);

	if (targetAddr >= end)
		return;

	remap_pfn_range(vma, targetAddr, pfn, size, PAGE_SHARED);
}

static uint64_t map_task_pages(struct task_struct* task, uint64_t addr, uint64_t size, int hugePages)
{
	struct mm_struct* mm = task->mm;
	uint64_t ret = 0;
	uint64_t pageSize = 0x1000;
	uint64_t pageMul = hugePages ? 512 : 1;
	int locked = 1;
	uint64_t nrPages = size / pageSize;
	struct page** pages = NULL;
	long gotPages = 0;
	size_t i;
	struct vm_area_struct* vma, *prevVma;
	uint64_t origPdp = addr >> 39;
	uint64_t targetPdp = origPdp - 0x20;
	uint64_t targetAddr = (addr & ~(origPdp << 39)) | (targetPdp << 39);
	vm_flags_t vmFlags = VM_READ | VM_WRITE | VM_SHARED;

	if (!mm || !_vm_area_free || !_vm_area_alloc || !_insert_vm_struct)
		return 0;

	pages = (struct page**)vmalloc(sizeof(struct page*) * nrPages);

	printk("vmread: checking 0x%llx pages\n", nrPages);

	down_write(&mm->mmap_sem);

	gotPages = get_user_pages_remote(task, mm, addr, nrPages, FOLL_GET, pages, NULL, &locked);

	printk("vmread: got 0x%lx pages\n", gotPages);

	if (gotPages > 0) {

		/* this should never occur in sane conditions, but better safe than deadlock */
		if (current->mm != mm)
			down_write(&current->mm->mmap_sem);

		vma = _vm_area_alloc(current->mm);
		prevVma = current->mm->mmap;

		if (!vma)
			goto cleanup;

		if (!prevVma) {
			_vm_area_free(vma);
			goto cleanup;
		}

		vma->vm_start = targetAddr;
		vma->vm_end = targetAddr + size;
		vma->vm_flags = vmFlags;
		vma->vm_page_prot = vm_get_page_prot(vmFlags);
		vma->vm_pgoff = 0;

		_insert_vm_struct(current->mm, vma);

		for (i = 0; i < gotPages; i += pageMul)
			map_part_of_vm(vma, targetAddr + pageSize * i, pageSize * pageMul, page_to_pfn(pages[i]));

		ret = targetAddr;

	cleanup:

		while (gotPages-- > 0)
			put_page(pages[gotPages]);


		if (current->mm != mm)
			up_write(&current->mm->mmap_sem);
	}

	if (locked)
		up_write(&mm->mmap_sem);

	return ret;
}

static void vmmap(ProcessData* data)
{
	struct task_struct* task = NULL;
	uint64_t addr = data->mapsStart;

	task = pid_task(find_vpid(data->pid), PIDTYPE_PID);

	if (!task)
		return;

	addr = map_task_pages(task, data->mapsStart, data->mapsSize, 0);

	if (addr) {
		data->mapsStart = addr;
		data->pid = task_pid_nr(current);
		printk("vmread: mapping successful!\n");
	}
}
