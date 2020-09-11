#include "mem.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef NO_ASSERTS
#include <assert.h>
#endif

#if (LMODE() == MODE_EXTERNAL() && !defined(KMOD_MEMMAP))
#define USE_PAGECACHE
#endif

/* For how long should the cached page be valid */
#ifndef VT_CACHE_TIME_MS
#define VT_CACHE_TIME_MS 1
#endif

static size_t vtCacheTimeMS = VT_CACHE_TIME_MS;

#define VT_CACHE_TIME_NS vtCacheTimeMS * 1000000ll

static const uint64_t PMASK = (~0xfull << 8) & 0xfffffffffull;
static void FillRWInfo(const ProcessData* data, uint64_t dirBase, RWInfo* info, int* count, uint64_t local, uint64_t remote, size_t len);
static int FillRWInfoMul(const ProcessData* data, uint64_t dirBase, RWInfo* origData, RWInfo* info, size_t count);
static int CalculateDataCount(RWInfo* info, size_t count);

ssize_t VMemRead(const ProcessData* data, uint64_t dirBase, uint64_t local, uint64_t remote, size_t size)
{
	if ((remote >> 12ull) == ((remote + size) >> 12ull))
		return MemRead(data, local, VTranslate(data, dirBase, remote), size);

	int dataCount = (int)((size - 1) / 0x1000) + 2;
	RWInfo rdataStack[MAX_BATCHED_RW];
	RWInfo* rdata = rdataStack;

	if (dataCount > MAX_BATCHED_RW)
		rdata = (RWInfo*)malloc(sizeof(RWInfo) * dataCount);

	FillRWInfo(data, dirBase, rdata, &dataCount, local, remote, size);
	ssize_t ret = MemReadMul(data, rdata, dataCount);

	if (dataCount > MAX_BATCHED_RW)
		free(rdata);

	return ret;
}

ssize_t VMemWrite(const ProcessData* data, uint64_t dirBase, uint64_t local, uint64_t remote, size_t size)
{
	if ((remote >> 12ull) == ((remote + size) >> 12ull))
		return MemWrite(data, local, VTranslate(data, dirBase, remote), size);

	int dataCount = (int)((size - 1) / 0x1000) + 2;
	RWInfo wdataStack[MAX_BATCHED_RW];
	RWInfo* wdata = wdataStack;

	if (dataCount > MAX_BATCHED_RW)
		wdata = (RWInfo*)malloc(sizeof(RWInfo) * dataCount);

	FillRWInfo(data, dirBase, wdata, &dataCount, local, remote, size);
	ssize_t ret = MemWriteMul(data, wdata, dataCount);

	if (dataCount > MAX_BATCHED_RW)
		free(wdata);

	return ret;
}

uint64_t VMemReadU64(const ProcessData* data, uint64_t dirBase, uint64_t remote)
{
	uint64_t dest;
	MemRead(data, (uint64_t)&dest, VTranslate(data, dirBase, remote), sizeof(uint64_t));
	return dest;
}

ssize_t VMemWriteU64(const ProcessData* data, uint64_t dirBase, uint64_t remote, uint64_t value)
{
	return MemRead(data, (uint64_t)&value, VTranslate(data, dirBase, remote), sizeof(uint64_t));
}

uint64_t MemReadU64(const ProcessData* data, uint64_t remote)
{
	uint64_t dest;
	MemRead(data, (uint64_t)&dest, remote, sizeof(uint64_t));
	return dest;
}

ssize_t MemWriteU64(const ProcessData* data, uint64_t remote, uint64_t value)
{
	return MemRead(data, (uint64_t)&value, remote, sizeof(uint64_t));
}

ssize_t VMemReadMul(const ProcessData* data, uint64_t dirBase, RWInfo* info, size_t num)
{
	int dataCount = CalculateDataCount(info, num);
	RWInfo readInfoStack[MAX_BATCHED_RW];
	RWInfo* readInfo = readInfoStack;

	if (num > MAX_BATCHED_RW)
		readInfo = (RWInfo*)malloc(sizeof(RWInfo) * num);

	dataCount = FillRWInfoMul(data, dirBase, info, readInfo, num);
	ssize_t ret = MemReadMul(data, readInfo, dataCount);

	if (num > MAX_BATCHED_RW)
		free(readInfo);

	return ret;
}

ssize_t VMemWriteMul(const ProcessData* data, uint64_t dirBase, RWInfo* info, size_t num)
{
	int dataCount = CalculateDataCount(info, num);
	RWInfo writeInfoStack[MAX_BATCHED_RW];
	RWInfo* writeInfo = writeInfoStack;

	if (num > MAX_BATCHED_RW)
			writeInfo = (RWInfo*)malloc(sizeof(RWInfo) * num);

	dataCount = FillRWInfoMul(data, dirBase, info, writeInfo, num);
	ssize_t ret = MemWriteMul(data, writeInfo, dataCount);

	if (num > MAX_BATCHED_RW)
		free(writeInfo);

	return ret;
}

/*
  This is used to cache the pages touched last bu reads of VTranslate, this increases the performance of external mode by at least 2x for multiple consequitive reads in common area. Cached page expires after a set interval which should be small enough not to cause very serious harm
*/

#if defined(USE_PAGECACHE) || !defined(NO_TLB)
static __thread struct timespec vtCurTime;
#endif

#ifdef USE_PAGECACHE
static __thread uint64_t vtCachePage[4] = { 0, 0, 0, 0 };
static __thread char vtCache[4][0x1000];
static __thread struct timespec vtCacheTime[4];
#endif

#ifndef NO_TLB
int vtTLBHits = 0;
int vtTLBMisses = 0;

typedef struct {
	uint64_t page;
	uint64_t dirBase;
	uint64_t translation;
	struct timespec resultTime;
} tlbentry_t;

// This makes the cache fit in 12 pages
#ifndef TLB_SIZE
#define TLB_SIZE 1024
#endif
static __thread tlbentry_t vtTlb[TLB_SIZE];
#endif

static size_t GetTlbIndex(uint64_t address) {
	return (address >> 12l) % TLB_SIZE;
}

static void VtUpdateCurTime()
{
#if defined(USE_PAGECACHE) || !defined(NO_TLB)
	clock_gettime(CLOCK_MONOTONIC_COARSE, &vtCurTime);
#endif
}

static uint64_t VtMemReadU64(const ProcessData* data, size_t idx, uint64_t address)
{
#ifdef USE_PAGECACHE
	uint64_t page = address & ~0xfff;

	uint64_t timeDiff = (vtCurTime.tv_sec - vtCacheTime[idx].tv_sec) * (uint64_t)1e9 + (vtCurTime.tv_nsec - vtCacheTime[idx].tv_nsec);

	if (vtCachePage[idx] != page || timeDiff >= VT_CACHE_TIME_NS) {
		MemRead(data, (uint64_t)vtCache[idx], page, 0x1000);
		vtCachePage[idx] = page;
		vtCacheTime[idx] = vtCurTime;
	}

	return *(uint64_t*)(void*)(vtCache[idx] + (address & 0xfff));
#else
	(void)idx;
	return MemReadU64(data, address);
#endif
}

static uint64_t VtCheckCachedResult(uint64_t inAddress, uint64_t dirBase)
{
	VtUpdateCurTime();
#ifndef NO_TLB
	tlbentry_t* tlb = &vtTlb[GetTlbIndex(inAddress)];
	if ((tlb->dirBase == dirBase) && (tlb->page == (inAddress & ~0xfff))) {
		uint64_t timeDiff = (vtCurTime.tv_sec - tlb->resultTime.tv_sec) * (uint64_t)1e9 + (vtCurTime.tv_nsec - tlb->resultTime.tv_nsec);

		if (timeDiff < VT_CACHE_TIME_NS) {
			vtTLBHits++;
			return tlb->translation | (inAddress & 0xfff);
		}
	}
#endif
	return 0;
}

static void VtUpdateCachedResult(uint64_t inAddress, uint64_t address, uint64_t dirBase)
{
#ifndef NO_TLB
	tlbentry_t* tlb = &vtTlb[GetTlbIndex(inAddress)];
	tlb->resultTime = vtCurTime;
	tlb->translation = address & ~0xfff;
	tlb->page = inAddress & ~0xfff;
	tlb->dirBase = dirBase;
	vtTLBMisses++;
#endif
}

/*
  Translates a virtual address to a physical one. This (most likely) is windows specific and might need extra work to work on Linux target.
*/

static uint64_t VTranslateInternal(const ProcessData* data, uint64_t dirBase, uint64_t address)
{
	uint64_t pageOffset = address & ~(~0ul << PAGE_OFFSET_SIZE);
	uint64_t pte = ((address >> 12) & (0x1ffll));
	uint64_t pt = ((address >> 21) & (0x1ffll));
	uint64_t pd = ((address >> 30) & (0x1ffll));
	uint64_t pdp = ((address >> 39) & (0x1ffll));

	uint64_t pdpe = VtMemReadU64(data, 0, dirBase + 8 * pdp);
	if (~pdpe & 1)
		return 0;

	uint64_t pde = VtMemReadU64(data, 1, (pdpe & PMASK) + 8 * pd);
	if (~pde & 1)
		return 0;

	/* 1GB large page, use pde's 12-34 bits */
	if (pde & 0x80)
		return (pde & (~0ull << 42 >> 12)) + (address & ~(~0ull << 30));

	uint64_t pteAddr = VtMemReadU64(data, 2, (pde & PMASK) + 8 * pt);
	if (~pteAddr & 1)
		return 0;

	/* 2MB large page */
	if (pteAddr & 0x80)
		return (pteAddr & PMASK) + (address & ~(~0ull << 21));

	address = VtMemReadU64(data, 3, (pteAddr & PMASK) + 8 * pte) & PMASK;

	if (!address)
		return 0;

	return address + pageOffset;
}

uint64_t VTranslate(const ProcessData* data, uint64_t dirBase, uint64_t address)
{
	dirBase &= ~0xf;

	uint64_t cachedVal = VtCheckCachedResult(address, dirBase);

	if (cachedVal)
		return cachedVal;

	cachedVal = VTranslateInternal(data, dirBase, address);

	VtUpdateCachedResult(address, cachedVal, dirBase);

	return cachedVal;
}

void SetMemCacheTime(size_t newTime)
{
	if (newTime >= 0)
		vtCacheTimeMS = newTime;
}

size_t GetDefaultMemCacheTime()
{
	return VT_CACHE_TIME_MS;
}

/* Static functions */

static int CalculateDataCount(RWInfo* info, size_t count)
{
	int ret = 0;

	for (size_t i = 0; i < count; i++)
		ret += ((info[i].remote + info[i].size - 1) >> 12ull) - (info[i].remote >> 12ull);

	return ret;
}

static int FillRWInfoMul(const ProcessData* data, uint64_t dirBase, RWInfo* origData, RWInfo* info, size_t count)
{
	int ret = 0;

	for (size_t i = 0; i < count; i++) {
		int lcount = 0;
		FillRWInfo(data, dirBase, info + ret, &lcount, origData[i].local, origData[i].remote, origData[i].size);
		ret += lcount;
	}
	return ret;
}

static void FillRWInfo(const ProcessData* data, uint64_t dirBase, RWInfo* info, int* count, uint64_t local, uint64_t remote, size_t len)
{
	memset(info, 0, sizeof(RWInfo) * *count);
	info[0].local = local;
	info[0].remote = VTranslate(data, dirBase, remote);
	info[0].size = 0x1000 - (remote & 0xfff);

#ifndef NO_ASSERTS
	assert(!((remote + info[0].size) & 0xfff));
#endif

	if (info[0].size > len)
		info[0].size = len;

	uint64_t curSize = info[0].size;

	uint64_t tlen = 0;

	int i = 1;
	for (; curSize < len; curSize += 0x1000) {
		info[i].local = local + curSize;
		info[i].remote = VTranslate(data, dirBase, remote + curSize);
		info[i].size = len - curSize;
		if (info[i].size > 0x1000)
		    info[i].size = 0x1000;
		if (info[i].remote) {
			tlen += info[i].size;
			i++;
		}
	}

	*count = i;
}
