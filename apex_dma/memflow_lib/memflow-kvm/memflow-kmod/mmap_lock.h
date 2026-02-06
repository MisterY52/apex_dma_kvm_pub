/* SPDX-License-Identifier: GPL-2.0 */

#ifndef MMAP_LOCK_H
#define MMAP_LOCK_H

#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,8,0)
static inline void mmap_write_lock(struct mm_struct *mm)
{
	down_write(&mm->mmap_sem);
}

static inline void mmap_write_unlock(struct mm_struct *mm)
{
	up_write(&mm->mmap_sem);
}
#else
#include <linux/mmap_lock.h>
#endif

#endif
