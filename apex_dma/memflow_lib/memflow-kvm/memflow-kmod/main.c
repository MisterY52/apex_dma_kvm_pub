// SPDX-License-Identifier: GPL-2.0
#include "vmtools.h"
#include <linux/types.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include "../mabi.h"
#include "main.h"
#include "kallsyms/kallsyms.h"
#include "kallsyms/kallsyms.c"
#include "kallsyms/ksyms.h"

MODULE_DESCRIPTION("memflow kernel module used to support KVM backend");
MODULE_AUTHOR("Heep");
MODULE_LICENSE("GPL");

static int memflow_init(void);
static void memflow_exit(void);

module_init(memflow_init);
module_exit(memflow_exit);

static long memflow_ioctl(struct file *filp, unsigned int cmd, unsigned long argp);

static const struct file_operations memflow_chardev_ops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = memflow_ioctl
};

static struct miscdevice memflow_dev = {
	MEMFLOW_IOCTL_MAGIC,
	"memflow",
	&memflow_chardev_ops
};


KSYMDEF(kvm_lock);
KSYMDEF(vm_list);

static int memflow_init(void)
{
	int r;

	if ((r = init_kallsyms()))
		return r;

	KSYMINIT_FAULT(kvm_lock);
	KSYMINIT_FAULT(vm_list);

	r = misc_register(&memflow_dev);

	if (r)
		return r;

	mprintk("initialized\n");

	return 0;
}

static void memflow_exit(void)
{
	misc_deregister(&memflow_dev);
	mprintk("uninitialized\n");
}

static long memflow_ioctl(struct file *filp, unsigned int cmd, unsigned long argp)
{
	pid_t target_pid = argp;

	switch (cmd) {
		case MEMFLOW_OPEN_VM:
			return open_vm(target_pid);
	}

	return -1;
}
