#ifndef PROCESSDATA_H
#define PROCESSDATA_H

typedef struct ProcessData
{
	uint64_t mapsStart;
	uint64_t mapsSize;
	pid_t pid;
} ProcessData;

#endif
