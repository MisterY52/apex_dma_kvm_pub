
#ifndef WINTOOLS_H
#define WINTOOLS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file wintools.h
 * @brief Defines the main vmread functions
 *
 * Provides all utilities and structures for Windows operating systems.
 */

#include "definitions.h"
#include "winstructs.h"

typedef struct WinOffsets
{
	int64_t apl;
	int64_t session;
	int64_t stackCount;
	int64_t imageFileName;
	int64_t dirBase;
	int64_t peb;
	int64_t peb32;
	int64_t threadListHead;
	int64_t threadListEntry;
	int64_t teb;
} WinOffsets;

typedef struct WinProc
{
	uint64_t process;
	uint64_t physProcess;
	uint64_t dirBase;
	uint64_t pid;
	char* name;
} WinProc;

typedef struct WinProcList
{
	WinProc* list;
	size_t size;
} WinProcList;

typedef struct WinExport
{
	char* name;
	uint64_t address;
} WinExport;

typedef struct WinExportList
{
	WinExport* list;
	size_t size;
} WinExportList;

typedef struct WinModule
{
	uint64_t baseAddress;
	uint64_t entryPoint;
	uint64_t sizeOfModule;
	char* name;
	short loadCount;
} WinModule;

typedef struct WinModuleList
{
	WinModule* list;
	size_t size;
} WinModuleList;

typedef struct WinCtx
{
	ProcessData process;
	WinOffsets offsets;
	uint64_t ntKernel;
	uint16_t ntVersion;
	uint32_t ntBuild;
	WinExportList ntExports;
	WinProc initialProcess;
} WinCtx;

/**
 * @brief Initialize the vmread context
 *
 * @param ctx context to be initialized
 * @param pid target process ID
 *
 * Initialize ctx by using pid. If pid is 0,
 * the function tries to determine it automatically.
 *
 * @return
 * 0 if initialization was successful;
 * otherwise, the return value is an error value.
 */
int InitializeContext(WinCtx* ctx, pid_t pid);

/**
 * @brief Free the vmread context
 *
 * @param ctx context to free
 *
 * It frees all the data inside the ctx making it free to be disposed
 *
 * @return
 * Always 0
 */
int FreeContext(WinCtx* ctx);

/**
 * @brief Get the NT header of a module
 *
 * @param ctx vmread context
 * @param process target process
 * @param address base address of the target module
 * @param header buffer to read the header to
 * @param is64Bit flag that returns if the module is 64-bit
 *
 * header has to be at least one page long, and @is64Bit can not be NULL.
 *
 * @return
 * Pointer to the NT header, if it was found;
 * otherwise @c NULL
 */
IMAGE_NT_HEADERS* GetNTHeader(const WinCtx* ctx, const WinProc* process, uint64_t address, uint8_t* header, uint8_t* is64Bit);

/**
 * @brief Parse module export table, writing them to the export list
 *
 * @param ctx vmread context
 * @param process target process
 * @param moduleBase base address of the module
 * @param exports address to the export table (parsed from the header)
 * @param outList the list that gets the data written to
 *
 * @return
 * 0 on success;
 * Otherwise a positive error number indicating stage of the failure
 */
int ParseExportTable(const WinCtx* ctx, const WinProc* process, uint64_t moduleBase, IMAGE_DATA_DIRECTORY* exports, WinExportList* outList);

/**
 * @brief Generate a module export list
 *
 * @param ctx vmread context
 * @param process target process
 * @param moduleBase base address
 * @param outList the output list
 *
 * @return
 * 0 on success;
 * Otherwise either -1, or a positive number indicating stage of the failure
 */
int GenerateExportList(const WinCtx* ctx, const WinProc* process, uint64_t moduleBase, WinExportList* outList);

/**
 * @brief Free the data inside the export list
 *
 * @param list list to be freed
 */
void FreeExportList(WinExportList list);

/**
 * @brief Get the address of a module export
 *
 * @param ctx vmread context
 * @param process target process
 * @param module base address of the module
 * @param procName target export name
 *
 * This function generates an export list and immediately frees it, so it is advised against using it extensively. See GetProcAddress.
 *
 * @return
 * Virtual address of the export, 0, if not found
 */
uint64_t GetProcAddress(const WinCtx* ctx, const WinProc* process, uint64_t module, const char* procName);

/**
 * @brief Find the proc address inside a given export list
 *
 * @param exports the list to be searched
 * @param procName target export name
 *
 * @return
 * Virtual address of the export, 0, if not found
 */
uint64_t FindProcAddress(const WinExportList exports, const char* procName);

/**
 * @brief Generate the list of processes
 *
 * @param ctx vmread context
 *
 * @return
 * A structure representing the process list
 */
WinProcList GenerateProcessList(const WinCtx* ctx);

/**
 * @brief Free the data inside a process list
 *
 * @param list the list to have its data freed
 */
void FreeProcessList(WinProcList list);

/**
 * @brief Generate the module list of a process
 *
 * @param ctx vmread context
 * @param process target process
 *
 * @return
 * A structure representing all modules loaded by the given process
 */
WinModuleList GenerateModuleList(const WinCtx* ctx, const WinProc* process);

/**
 * @brief GenerateKernelModuleList
 *
 * @param ctx vmread context
 *
 * @return
 * A structure representing all modules loaded by the kernel
 */
WinModuleList GenerateKernelModuleList(const WinCtx* ctx);

/**
 * @brief Free a given module list
 *
 * @param list list to have its data freed in
 */
void FreeModuleList(WinModuleList list);

/**
 * @brief Find the module by a given name
 *
 * @param list list to perform search in
 * @param moduleName target module name
 *
 * @return
 * A pointer to the found module information, @c NULL if not found
 */
const WinModule* GetModuleInfo(const WinModuleList list, const char* moduleName);

/**
 * @brief Get the process environment block
 *
 * @param ctx vmread context
 * @param process target process
 *
 * Note that there is no error checking in this function. Even though PEB should always exist,
 * the returned data may be garbage
 *
 * @return
 * The environment block
 */
PEB GetPeb(const WinCtx* ctx, const WinProc* process);

/**
 * @brief Get the process environment block (32-bit version)
 *
 * @param ctx vmread context
 * @param process target process
 *
 * Note that there is no error checking in this function. Even though PEB should always exist,
 * the returned data may be garbage
 *
 * @return
 * The environment block
 */
PEB32 GetPeb32(const WinCtx* ctx, const WinProc* process);

#ifdef __cplusplus
}
#endif

#endif
