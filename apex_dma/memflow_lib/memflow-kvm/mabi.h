#ifndef MABI_H
#define MABI_H

#include <linux/types.h>
#include <linux/ioctl.h>

typedef struct vm_memslot {
	__aligned_u64 base;
	__aligned_u64 host_base;
	__aligned_u64 map_size;
} vm_memslot_t;

typedef struct vm_info {
	__kernel_pid_t userspace_pid;
	__u32 slot_count;
	struct vm_memslot *slots;
} vm_info_t;

typedef struct vm_map_info {
	__u32 slot_count;
	struct vm_memslot *slots;
} vm_map_info_t;

#define MEMFLOW_IOCTL_MAGIC 0x6d

#define MEMFLOW_OPEN_VM _IOR(MEMFLOW_IOCTL_MAGIC, 0, __kernel_pid_t)
#define MEMFLOW_VM_INFO _IOWR(MEMFLOW_IOCTL_MAGIC, 1, vm_info_t)
#define MEMFLOW_MAP_VM _IOWR(MEMFLOW_IOCTL_MAGIC, 2, vm_map_info_t)

#endif
