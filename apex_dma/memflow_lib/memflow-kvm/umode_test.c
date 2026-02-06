#include "mabi.h"
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <alloca.h>
#include <errno.h>
#include <sys/mman.h>
#include <string.h>

#define MAX_MEMSLOTS 64

int main()
{
	int memflow_fd = open("/dev/memflow", O_RDONLY);

	if (memflow_fd == -1) {
		printf("/dev/memflow open failed\n");
		return -1;
	}

	printf("memflow_fd %x\n", memflow_fd);

	int vm_fd = ioctl(memflow_fd, MEMFLOW_OPEN_VM, 0);

	if (vm_fd == -1) {
		printf("MEMFLOW_OPEN_VM failed\n");
		return -1;
	}

	printf("vm_fd %d\n", vm_fd);

	{
		vm_info_t *vm_info = alloca(sizeof(vm_info_t) + sizeof(vm_memslot_t) * MAX_MEMSLOTS);
		vm_info->slots = vm_info + 1;
		vm_info->slot_count = MAX_MEMSLOTS;

		if (ioctl(vm_fd, MEMFLOW_VM_INFO, vm_info)) {
			printf("MEMFLOW_VM_INFO failed %d\n", errno);
			return -1;
		}

		printf("kvm pid: %d\n", (int)vm_info->userspace_pid);
		printf("Memory maps (count=%u):\n", vm_info->slot_count);

		for (int i = 0; i < vm_info->slot_count; i++) {
			vm_memslot_t *slot = vm_info->slots + i;
			printf("%d %llx->%llx (%llx)\n", i, slot->base, slot->host_base, slot->map_size);
		}
	}

	{
		vm_map_info_t *vm_info = alloca(sizeof(vm_map_info_t) + sizeof(vm_memslot_t) * MAX_MEMSLOTS);
		vm_info->slots = vm_info + 1;
		vm_info->slot_count = MAX_MEMSLOTS;

		int vm_map_fd = ioctl(vm_fd, MEMFLOW_MAP_VM, vm_info);

		if (vm_map_fd < 0) {
			printf("MEMFLOW_MAP_VM failed %d\n", errno);
			return -1;
		}

		printf("MEMFLOW_MAP_VM fd %d\n", vm_map_fd);

		getchar();

		printf("Memory maps (count=%u):\n", vm_info->slot_count);

		for (int i = 0; i < vm_info->slot_count; i++) {
			vm_memslot_t *slot = vm_info->slots + i;
			printf("%d %llx->%llx (%llx)\n", i, slot->base, slot->host_base, slot->map_size);
			memset((void *)slot->host_base, 0, slot->map_size);
		}

		close(vm_map_fd);

		printf("Closed vm_map_fd");

		getchar();
	}

	return 0;
}
