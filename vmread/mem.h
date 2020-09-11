#ifndef MEM_H
#define MEM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file mem.h
 * @brief Defines memory operations
 *
 * Provides all necessary methods to read and write memory in a VM
 */

#include "definitions.h"

#define PAGE_OFFSET_SIZE 12

#define MAX_BATCHED_RW 1024

typedef struct RWInfo
{
	uint64_t local;
	uint64_t remote;
	size_t size;
} RWInfo;

/**
 * @brief Read a piece of data in physical VM address space
 *
 * @param data VM process data
 * @param local local data address
 * @param remote remote data address
 * @param size size of data
 *
 * @return
 * Data moved on success;
 * -1 otherwise
 */
ssize_t MemRead(const ProcessData* data, uint64_t local, uint64_t remote, size_t size);

/**
 * @brief Write a piece of data in physical VM address space
 *
 * @param data VM process data
 * @param local local data address
 * @param remote remote data address
 * @param size size of data
 *
 * @return
 * Data moved on success;
 * -1 otherwise
 */
ssize_t MemWrite(const ProcessData* data, uint64_t local, uint64_t remote, size_t size);

/**
 * @brief Read multiple pieces of data in physical VM address space
 *
 * @param data VM process data
 * @param info list of information for RW operations
 * @param num number of info atoms
 *
 * @return
 * Data moved on success;
 * -1 otherwise
 */
ssize_t MemReadMul(const ProcessData* data, RWInfo* info, size_t num);

/**
 * @brief Write multiple pieces of data in physical VM address space
 *
 * @param data VM process data
 * @param info list of information for RW operations
 * @param num number of info atoms
 *
 * @return
 * Data moved on success;
 * -1 otherwise
 */
ssize_t MemWriteMul(const ProcessData* data, RWInfo* info, size_t num);

/**
 * @brief Read a unsigned 64-bit integer in virtual VM address space
 *
 * @param data VM process data
 * @param dirBase page table directory base of a process
 * @param remote remote data address
 *
 * @return
 * Read value, undefined on failure
 */
uint64_t VMemReadU64(const ProcessData* data, uint64_t dirBase, uint64_t remote);

/**
 * @brief Write a unsigned 64-bit integer in virtual VM address space
 *
 * @param data VM process data
 * @param dirBase page table directory base of a process
 * @param remote remote data address
 * @param value value to be written
 *
 * @return
 * 8 on success;
 * -1 on failure
 */
ssize_t VMemWriteU64(const ProcessData* data, uint64_t dirBase, uint64_t remote, uint64_t value);

/**
 * @brief Read a unsigned 64-bit integer in physical VM address space
 *
 * @param data VM process data
 * @param remote remote data address
 *
 * @return
 * Read value, undefined on failure
 */
uint64_t MemReadU64(const ProcessData* data, uint64_t remote);

/**
 * @brief Write a unsigned 64-bit integer in physical VM address space
 *
 * @param data VM process data
 * @param remote remote data address
 * @param value value to be written
 *
 * @return
 * 8 on success;
 * -1 on failure
 */
ssize_t MemWriteU64(const ProcessData* data, uint64_t remote, uint64_t value);

/**
 * @brief Read data in virtual VM address space
 *
 * @param data VM process data
 * @param dirBase page table directory base of a process
 * @param local local data address
 * @param remote remote data address
 * @param size size of data
 *
 * @return
 * Data moved on success;
 * -1 otherwise
 */
ssize_t VMemRead(const ProcessData* data, uint64_t dirBase, uint64_t local, uint64_t remote, size_t size);

/**
 * @brief Write data in virtual addresss space
 *
 * @param data VM process data
 * @param dirBase page table directory base of a process
 * @param local local data address
 * @param remote remote data address
 * @param size size of data
 *
 * @return
 * Data moved on success;
 * -1 otherwise
 */
ssize_t VMemWrite(const ProcessData* data, uint64_t dirBase, uint64_t local, uint64_t remote, size_t size);

/**
 * @brief Read multiple pieces of data in virtual VM address space
 *
 * @param data VM process data
 * @param dirBase page table directory base of a process
 * @param info list of information for RW operations
 * @param num number of info atoms
 *
 * @return
 * Data moved on success;
 * -1 otherwise
 */
ssize_t VMemReadMul(const ProcessData* data, uint64_t dirBase, RWInfo* info, size_t num);

/**
 * @brief Write multiple pieces of data in virtual VM address space
 *
 * @param data VM process data
 * @param dirBase page table directory base of a process
 * @param info list of information for RW operations
 * @param num number of info atoms
 *
 * @return
 * Data moved on success;
 * -1 otherwise
 */
ssize_t VMemWriteMul(const ProcessData* data, uint64_t dirBase, RWInfo* info, size_t num);

/**
 * @brief Translate a virtual VM address into a physical one
 *
 * @param data VM process data
 * @param dirBase page table directory base of a process
 * @param address virtual address to translate
 *
 * @return
 * Translated linear address;
 * 0 otherwise
 */
uint64_t VTranslate(const ProcessData* data, uint64_t dirBase, uint64_t address);

/**
 * @brief Set translation cache validity time in msecs
 *
 * @param newTime new validity length for a cache entry
 *
 * Defines for how long translation caches (TLB and page buffer) should be valid. Higher values lead to higher
 * performance, but could potentially lead to incorrect translation if the page tables update in that period.
 * Especially dangerous if write operations are to be performed.
 */
void SetMemCacheTime(size_t newTime);

/**
 * @brief Get the default cache validity
 *
 * @return
 * Default cache validity time
 */
size_t GetDefaultMemCacheTime();

#ifdef __cplusplus
}
#endif

#endif
