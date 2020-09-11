#include "mem.h"
#include <stdlib.h>
#include <sys/uio.h>
#include <stdio.h>
#include <unistd.h>

/* Memory read implementation using linux process_vm_ functions */

/*
  On windows, first 2^31 physical bytes are really allocated for various PCI device mappings and do not point to actual physical RAM (mmapped region in QEMU case).
  It also differs on Windows XP. This should be solved by injecting code into the kernel to retrieve the actual physical memory map of the system
*/
extern uint64_t KFIXC;
extern uint64_t KFIXO;
uint64_t KFIXC = 0x80000000;
uint64_t KFIXO = 0x80000000;
#define KFIX2(x) ((x) < KFIXC ? (x) : ((x) - KFIXO))

ssize_t process_vm_readv(pid_t pid,
						 const struct iovec *local_iov,
						 unsigned long liovcnt,
						 const struct iovec *remote_iov,
						 unsigned long riovcnt,
						 unsigned long flags);

ssize_t process_vm_writev(pid_t pid,
						  const struct iovec *local_iov,
						  unsigned long liovcnt,
						  const struct iovec *remote_iov,
						  unsigned long riovcnt,
						  unsigned long flags);

ssize_t MemRead(const ProcessData* data, uint64_t localAddr, uint64_t remoteAddr, size_t len)
{
	struct iovec local;
	struct iovec remote;
	local.iov_base = (void*)localAddr;
	local.iov_len = len;
	remote.iov_base = (void*)(data->mapsStart + KFIX2(remoteAddr));
	remote.iov_len = len;
	return process_vm_readv(data->pid, &local, 1, &remote, 1, 0);
}

ssize_t MemReadMul(const ProcessData* data, RWInfo* rdata, size_t num)
{
	struct iovec local[__IOV_MAX];
	struct iovec remote[__IOV_MAX];
	size_t i = 0;
	size_t startRead = 0;

	ssize_t ret = 0;

	for (i = 0; i < num; i++) {
		local[i - startRead].iov_base = (void*)rdata[i].local;
		local[i - startRead].iov_len = rdata[i].size;
		remote[i - startRead].iov_base = (void*)(data->mapsStart + KFIX2(rdata[i].remote));
		remote[i - startRead].iov_len = rdata[i].size;

		if (i - startRead + 1 >= __IOV_MAX) {
			ret = process_vm_readv(data->pid, local, __IOV_MAX, remote, __IOV_MAX, 0);
			if (ret == -1)
				return ret;
			startRead = i + 1;
		}
	}

	if (i != startRead)
	    ret = process_vm_readv(data->pid, local, i - startRead, remote, i - startRead, 0);

	return ret;
}

ssize_t MemWrite(const ProcessData* data, uint64_t localAddr, uint64_t remoteAddr, size_t len)
{
	struct iovec local;
	struct iovec remote;
	local.iov_base = (void*)localAddr;
	local.iov_len = len;
	remote.iov_base = (void*)(data->mapsStart + KFIX2(remoteAddr));
	remote.iov_len = len;
	return process_vm_writev(data->pid, &local, 1, &remote, 1, 0);
}

ssize_t MemWriteMul(const ProcessData* data, RWInfo* wdata, size_t num)
{
	struct iovec local[__IOV_MAX];
	struct iovec remote[__IOV_MAX];
	size_t i;
	size_t startWrite = 0;

	ssize_t ret = 0;

	for (i = 0; i < num; i++) {
		local[i - startWrite].iov_base = (void*)wdata[i].local;
		local[i - startWrite].iov_len = wdata[i].size;
		remote[i - startWrite].iov_base = (void*)(data->mapsStart + KFIX2(wdata[i].remote));
		remote[i - startWrite].iov_len = wdata[i].size;

		if (i - startWrite >= (size_t)__IOV_MAX) {
			ret = process_vm_writev(data->pid, local, __IOV_MAX, remote, __IOV_MAX, 0);
			if (ret == -1)
				return ret;
			startWrite = i + 1;
		}
	}

	if (i != startWrite)
		ret = process_vm_writev(data->pid, local, i - startWrite, remote, i - startWrite, 0);

	return ret;
}
