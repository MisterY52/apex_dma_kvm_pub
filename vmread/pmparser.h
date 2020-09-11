/*
 @Author	: ouadimjamal@gmail.com
 @date		: December 2015
Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  No representations are made about the suitability of this
software for any purpose.  It is provided "as is" without express or
implied warranty.
 */

#ifndef PMPARSER_H
#define PMPARSER_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

/**
 * procmaps_struct
 * @desc hold all the information about an area in the process's  VM
 */
typedef struct procmaps_struct{
	void* addr_start;
	void* addr_end;
	unsigned long length;

	char perm[5];
	short is_r;
	short is_w;
	short is_x;
	short is_p;

	long offset;
	char dev[12];
	int inode;

	char pathname[600];

	struct procmaps_struct* next;
} procmaps_struct;

extern procmaps_struct* g_last_head;
extern procmaps_struct* g_current;

void _pmparser_split_line(
		char*buf,char*addr1,char*addr2,
		char*perm,char* offset,char* device,char*inode,
		char* pathname);

/**
 * pmparser_parse
 * @param pid the process id whose memory map to be parser. the current process if pid<0
 * @return list of procmaps_struct structers
 */
procmaps_struct* pmparser_parse(int pid);

/**
 * pmparser_next
 * @description move between areas
 */
procmaps_struct* pmparser_next(void);

/**
 * pmparser_free
 * @description should be called at the end to free the resources
 * @param maps_list the head of the list to be freed
 */
void pmparser_free(procmaps_struct* maps_list);

#ifdef __cplusplus
}
#endif

#endif
