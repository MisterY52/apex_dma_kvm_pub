/* SPDX-License-Identifier: GPL-2.0 */

#ifndef MEMFLOW_H
#define MEMFLOW_H

#include <linux/printk.h>
#define mprintk(format, ...) printk(KBUILD_MODNAME": "format, ##__VA_ARGS__)

#endif
