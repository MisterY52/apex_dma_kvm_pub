#include "mem.h"
#include <string.h>

/* Implementation for direct memory reads, used when injected into the QEMU process */

extern uint64_t KFIXC;
extern uint64_t KFIXO;
uint64_t KFIXC = 0x80000000;
uint64_t KFIXO = 0x80000000;
#define KFIX2(x) ((x) < KFIXC ? (x) : ((x) - KFIXO))

ssize_t MemRead(const ProcessData* data, uint64_t localAddr, uint64_t remoteAddr, size_t len)
{
	uint64_t remote = KFIX2(remoteAddr);
	if (remote >= data->mapsSize - len)
		return -1;
	memcpy((void*)localAddr, (void*)(remote + data->mapsStart), len);
	return len;
}

ssize_t MemReadMul(const ProcessData* data, RWInfo* rdata, size_t num)
{
    ssize_t flen = 0;
	size_t i;
	for (i = 0; i < num; i++) {
		uint64_t remote = KFIX2(rdata[i].remote);
		if (remote >= data->mapsSize - rdata[i].size)
			return -1;
		memcpy((void*)rdata[i].local, (void*)(remote + data->mapsStart), rdata[i].size);
		flen += rdata[i].size;
	}
	return flen;
}

ssize_t MemWrite(const ProcessData* data, uint64_t localAddr, uint64_t remoteAddr, size_t len)
{
	uint64_t remote = KFIX2(remoteAddr);
	if (remote >= data->mapsSize - len)
		return -1;
	memcpy((void*)(remote + data->mapsStart), (void*)localAddr, len);
	return len;
}

ssize_t MemWriteMul(const ProcessData* data, RWInfo* wdata, size_t num)
{
	ssize_t flen = 0;
	size_t i;
	for (i = 0; i < num; i++) {
		uint64_t remote = KFIX2(wdata[i].remote);
		if (remote >= data->mapsSize - wdata[i].size)
			return -1;
		memcpy((void*)(remote + data->mapsStart), (void*)wdata[i].local, wdata[i].size);
		flen += wdata[i].size;
	}
	return flen;
}
