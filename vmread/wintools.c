#include "wintools.h"
#include "mem.h"
#include "pmparser.h"
#include <errno.h>
#include <string.h>

#ifdef KMOD_MEMMAP
#include "kmem.h"
#include <sys/mman.h>
#include <sys/ioctl.h>
#endif

#if (LMODE() != MODE_QEMU_INJECT())
#include <sys/types.h>
#include <dirent.h>
#endif

char* strdup(const char*);

static int CheckLow(const WinCtx* ctx, uint64_t* pml4, uint64_t* kernelEntry);
static void FindNTKernel(WinCtx* ctx, uint64_t kernelEntry);
static uint16_t GetNTVersion(const WinCtx* ctx);
static uint32_t GetNTBuild(const WinCtx* ctx);
static int SetupOffsets(WinCtx* ctx);
static WinModule GetBaseModule(const WinCtx* ctx, const WinProc* process);
static void FillAnyModuleList64(const WinCtx* ctx, const WinProc* process, WinModuleList* list, size_t* maxSize, uint64_t head, int inMemoryOrder);
static void FillModuleList64(const WinCtx* ctx, const WinProc* process, WinModuleList* list, size_t* maxSize, char* x86);
static void FillModuleList32(const WinCtx* ctx, const WinProc* process, WinModuleList* list, size_t* maxSize);

extern uint64_t KFIXC;
extern uint64_t KFIXO;

FILE* vmread_dfile = NULL;

#ifndef HEADER_SIZE
#define HEADER_SIZE 0x1000
#endif

#if (LMODE() != MODE_QEMU_INJECT())
static int RecursFind(const char* path, int level) {
	if (level > 2)
		return 0;

	DIR* dir;
	struct dirent* entry;
	int ret = 0;

	if (!(dir = opendir(path)))
		return 0;

	while (!ret && (entry = readdir(dir))) {
		char npath[512];
		if (entry->d_type == DT_DIR && ((level == 1 && entry->d_name[0] == 'f') ||
					(entry->d_name[0] >= '0' && entry->d_name[0] <= '9'))) {
			snprintf(npath, sizeof(npath), "%s/%s", path, entry->d_name);
			ret = RecursFind(npath, level + 1);
		} else if (level >= 2) {
			snprintf(npath, sizeof(npath), "%s/%s", path, entry->d_name);
			uint64_t dirv = 0;
			const uint64_t kvm = *(uint64_t*)(void*)"/dev/kvm";
			if (readlink(npath, (char*)&dirv, 8) == 8 && dirv == kvm)
				sscanf(path, "/proc/%d/", &ret);
		}
	}
	closedir(dir);

	return ret;
}
#endif

static pid_t FindKVMProcess()
{
#if (LMODE() == MODE_QEMU_INJECT())
	return getpid();
#else
	pid_t ret = RecursFind("/proc", 0);
	return ret > 0 ? ret : 0;
#endif
}

int InitializeContext(WinCtx* ctx, pid_t pid)
{
	memset(ctx, 0, sizeof(WinCtx));

	uint64_t pml4, kernelEntry;

	if (pid == 0 && (pid = FindKVMProcess()) == 0)
		return -1;

	procmaps_struct* maps = pmparser_parse(pid);

	if (!maps)
		return 1;

	procmaps_struct* tempMaps = NULL;
	procmaps_struct* largestMaps = NULL;

	while ((tempMaps = pmparser_next()) != NULL)
		if (!largestMaps || tempMaps->length > largestMaps->length)
			largestMaps = tempMaps;

	if (!largestMaps)
		return 2;

	ctx->process.pid = pid;
	ctx->process.mapsStart = (uint64_t)largestMaps->addr_start;
	ctx->process.mapsSize = largestMaps->length;

	pmparser_free(maps);

#ifdef KMOD_MEMMAP
	MSG(2, "Mapping VM memory, this will take a second...\n");

	int fd = open("/proc/vmread", O_RDWR);

	if (fd != -1) {
		int ret = ioctl(fd, VMREAD_IOCTL_MAPVMMEM, &ctx->process);
		close(fd);
		if (ret)
			return 101;
	} else
		return 100;
#endif

	MSG(2, "Mem:\t%lx\t| Size:\t%lx\n", ctx->process.mapsStart, ctx->process.mapsSize);

	if (!CheckLow(ctx, &pml4, &kernelEntry))
		return 3;

	MSG(2, "PML4:\t%lx\t| KernelEntry:\t%lx\n", pml4, kernelEntry);

	ctx->initialProcess.dirBase = pml4;
	FindNTKernel(ctx, kernelEntry);

	if (!ctx->ntKernel) {
		/* Test in case we are running XP (QEMU AddressSpace is different) */
#if (LMODE() != MODE_DMA())
		KFIXC = 0x40000000ll * 4;
		KFIXO = 0x40000000;
		FindNTKernel(ctx, kernelEntry);
#endif

		if (!ctx->ntKernel)
			return 4;
	}

	MSG(2, "Kernel Base:\t%lx (%lx)\n", ctx->ntKernel, VTranslate(&ctx->process, ctx->initialProcess.dirBase, ctx->ntKernel));

	uint64_t initialSystemProcess = FindProcAddress(ctx->ntExports, "PsInitialSystemProcess");

	if (!initialSystemProcess)
		return 6;

	MSG(2, "PsInitialSystemProcess:\t%lx (%lx)\n", initialSystemProcess, VTranslate(&ctx->process, ctx->initialProcess.dirBase, initialSystemProcess));
	VMemRead(&ctx->process, ctx->initialProcess.dirBase, (uint64_t)&ctx->initialProcess.process, initialSystemProcess, sizeof(uint64_t));

	if (!ctx->initialProcess.process)
		return 7;

	ctx->initialProcess.physProcess = VTranslate(&ctx->process, ctx->initialProcess.dirBase, ctx->initialProcess.process);
	MSG(2, "System (PID 4):\t%lx (%lx)\n", ctx->initialProcess.process, ctx->initialProcess.physProcess);

	ctx->ntVersion = GetNTVersion(ctx);

	if (!ctx->ntVersion)
		return 8;

	MSG(2, "NT Version:\t%hu\n", ctx->ntVersion);

	ctx->ntBuild = GetNTBuild(ctx);

	if (!ctx->ntBuild)
		return 8;

	MSG(2, "NT Build:\t%u\n", ctx->ntBuild);

	if (SetupOffsets(ctx))
		return 9;

	return 0;
}

int FreeContext(WinCtx* ctx)
{
	FreeExportList(ctx->ntExports);
	return 0;
}

IMAGE_NT_HEADERS* GetNTHeader(const WinCtx* ctx, const WinProc* process, uint64_t address, uint8_t* header, uint8_t* is64Bit)
{
	if (VMemRead(&ctx->process, process->dirBase, (uint64_t)header, address, HEADER_SIZE) == -1)
		return NULL;

	//TODO: Allow the compiler to properly handle alignment
	IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)(void*)header;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return NULL;

	IMAGE_NT_HEADERS* ntHeader = (IMAGE_NT_HEADERS*)(void*)(header + dosHeader->e_lfanew);
	if ((uint8_t*)ntHeader - header > HEADER_SIZE - 0x200 || ntHeader->Signature != IMAGE_NT_SIGNATURE)
		return NULL;

	if(ntHeader->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC && ntHeader->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
		return NULL;

	*is64Bit = ntHeader->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC;

	return ntHeader;
}

int ParseExportTable(const WinCtx* ctx, const WinProc* process, uint64_t moduleBase, IMAGE_DATA_DIRECTORY* exports, WinExportList* outList)
{
	if (exports->Size < sizeof(IMAGE_EXPORT_DIRECTORY) || exports->Size > 0x7fffff || exports->VirtualAddress == moduleBase)
		return 1;

	char* buf = (char*)malloc(exports->Size + 1);

	IMAGE_EXPORT_DIRECTORY* exportDir = (IMAGE_EXPORT_DIRECTORY*)(void*)buf;
	if (VMemRead(&ctx->process, process->dirBase, (uint64_t)buf, moduleBase + exports->VirtualAddress, exports->Size) == -1) {
		free(buf);
		return 2;
	}
	buf[exports->Size] = 0;
	if (!exportDir->NumberOfNames || !exportDir->AddressOfNames) {
		free(buf);
		return 3;
	}

	uint32_t exportOffset = exports->VirtualAddress;

	uint32_t* names = (uint32_t*)(void*)(buf + exportDir->AddressOfNames - exportOffset);
	if (exportDir->AddressOfNames - exportOffset + exportDir->NumberOfNames * sizeof(uint32_t) > exports->Size) {
		free(buf);
		return 4;
	}
	uint16_t* ordinals = (uint16_t*)(void*)(buf + exportDir->AddressOfNameOrdinals - exportOffset);
	if (exportDir->AddressOfNameOrdinals - exportOffset + exportDir->NumberOfNames * sizeof(uint16_t) > exports->Size) {
		free(buf);
		return 5;
	}
	uint32_t* functions = (uint32_t*)(void*)(buf + exportDir->AddressOfFunctions - exportOffset);
	if (exportDir->AddressOfFunctions - exportOffset + exportDir->NumberOfFunctions * sizeof(uint32_t) > exports->Size) {
		free(buf);
		return 6;
	}

	outList->size = exportDir->NumberOfNames;
	outList->list = (WinExport*)malloc(sizeof(WinExport) * outList->size);

	size_t sz = 0;

	for (uint32_t i = 0; i < exportDir->NumberOfNames; i++) {
		if (names[i] > exports->Size + exportOffset || names[i] < exportOffset || ordinals[i] > exportDir->NumberOfNames)
			continue;
		outList->list[sz].name = strdup(buf + names[i] - exportOffset);
		outList->list[sz].address = moduleBase + functions[ordinals[i]];
		sz++;
	}

	outList->size = sz;

	free(buf);

	return 0;
}

int GenerateExportList(const WinCtx* ctx, const WinProc* process, uint64_t moduleBase, WinExportList* outList)
{
	uint8_t is64 = 0;
	uint8_t headerBuf[HEADER_SIZE];
	int ret = 0;

	IMAGE_NT_HEADERS64* ntHeader64 = GetNTHeader(ctx, process, moduleBase, headerBuf, &is64);

	if (!ntHeader64)
		return -1;

	IMAGE_NT_HEADERS32* ntHeader32 = (IMAGE_NT_HEADERS32*)ntHeader64;

	IMAGE_DATA_DIRECTORY* exportTable = NULL;
	if (is64)
		exportTable = ntHeader64->OptionalHeader.DataDirectory + IMAGE_DIRECTORY_ENTRY_EXPORT;
	else
		exportTable = ntHeader32->OptionalHeader.DataDirectory + IMAGE_DIRECTORY_ENTRY_EXPORT;

	ret = ParseExportTable(ctx, process, moduleBase, exportTable, outList);

	return ret;
}

void FreeExportList(WinExportList list)
{
	if (!list.list)
		return;

	for (uint32_t i = 0; i < list.size; i++)
		free((char*)list.list[i].name);

	free(list.list);
	list.list = NULL;
}

/* TODO: do without the table parsing, we are just wasting a lot of string duplication etc. */
uint64_t GetProcAddress(const WinCtx* ctx, const WinProc* process, uint64_t module, const char* procName)
{
	WinExportList exports;

	if (GenerateExportList(ctx, process, module, &exports))
		return 1;

	uint64_t ret = FindProcAddress(exports, procName);
	FreeExportList(exports);
	return ret;
}

uint64_t FindProcAddress(const WinExportList exports, const char* procName)
{
	for (uint32_t i = 0; i < exports.size; i++)
		if (!strcmp(procName, exports.list[i].name))
			return exports.list[i].address;
	return 0;
}

WinProcList GenerateProcessList(const WinCtx* ctx)
{
	WinProcList list;

	uint64_t curProc = ctx->initialProcess.physProcess;
	uint64_t virtProcess = ctx->initialProcess.process;

	list.list = (WinProc*)malloc(sizeof(WinProc) * 25);
	list.size = 0;
	size_t maxSize = 25;

	while (!list.size || (curProc != ctx->initialProcess.physProcess && virtProcess != ctx->initialProcess.process)) {
		uint64_t stackCount = MemReadU64(&ctx->process, curProc + ctx->offsets.stackCount);
		uint64_t dirBase = MemReadU64(&ctx->process, curProc + ctx->offsets.dirBase);
		uint64_t pid = MemReadU64(&ctx->process, curProc + ctx->offsets.apl - 8);

		/* The end of the process list usually has corrupted values, some sort of address, and we avoid the issue by checking the PID (which shouldn't be over 32 bit limit anyways) */
		if (pid < 1u << 31 && stackCount) {
			list.list[list.size] = (WinProc){
				.process = virtProcess,
				.physProcess = curProc,
				.dirBase = dirBase,
				.pid = pid,
			};

			WinModule baseMod = GetBaseModule(ctx, &list.list[list.size]);
			if (baseMod.name)
				list.list[list.size].name = baseMod.name;
			else {
				list.list[list.size].name = (char*)malloc(16);
				MemRead(&ctx->process, (uint64_t)list.list[list.size].name, curProc + ctx->offsets.imageFileName, 15);
				list.list[list.size].name[15] = '\0';
			}

			list.size++;
			if (list.size > 1000 || pid == 0)
				break;
		}

		if (list.size >= maxSize) {
			maxSize = list.size * 2;
			WinProc* newProc = (WinProc*)realloc(list.list, sizeof(WinProc) * maxSize);
			if (!newProc)
				break;
			list.list = newProc;
		}

		virtProcess = MemReadU64(&ctx->process, curProc + ctx->offsets.apl) - ctx->offsets.apl;
		if (!virtProcess)
			break;

		curProc = VTranslate(&ctx->process, dirBase, virtProcess);
		if (!curProc)
			break;
	}

	return list;
}

void FreeProcessList(WinProcList list)
{
	size_t i;

	if (list.list) {
		for (i = 0; i < list.size; i++)
			free(list.list[i].name);
		free(list.list);
	}
}

WinModuleList GenerateModuleList(const WinCtx* ctx, const WinProc* process)
{
	WinModuleList list;
	list.size = 0;
	list.list = (WinModule*)malloc(sizeof(WinModule) * 25);

	size_t maxSize = 25;
	char x86 = 0;

	FillModuleList64(ctx, process, &list, &maxSize, &x86);

	if (x86)
		FillModuleList32(ctx, process, &list, &maxSize);

	return list;
}

WinModuleList GenerateKernelModuleList(const WinCtx* ctx)
{
	WinModuleList list;
	list.size = 0;
	list.list = (WinModule*)malloc(sizeof(WinModule) * 25);

	size_t maxSize = 25;
	uint64_t psLoadedModuleList = FindProcAddress(ctx->ntExports, "PsLoadedModuleList");

	FillAnyModuleList64(ctx, &ctx->initialProcess, &list, &maxSize, psLoadedModuleList, 0);

	return list;
}

void FreeModuleList(WinModuleList list)
{
	if (!list.list)
		return;
	for (size_t i = 0; i < list.size; i++)
		free(list.list[i].name);
	free(list.list);
	list.list = NULL;
}

const WinModule* GetModuleInfo(const WinModuleList list, const char* moduleName)
{
	for (size_t i = 0; i < list.size; i++)
		if (!strcmp(list.list[i].name, moduleName))
			return list.list + i;
	return NULL;
}

PEB GetPeb(const WinCtx* ctx, const WinProc* process)
{
	PEB peb;
	uint64_t ppeb = MemReadU64(&ctx->process, process->physProcess + ctx->offsets.peb);
	VMemRead(&ctx->process, process->dirBase, (uint64_t)&peb, ppeb, sizeof(PEB));
	return peb;
}

PEB32 GetPeb32(const WinCtx* ctx, const WinProc* process)
{
	PEB32 peb;
	uint64_t ethread = MemReadU64(&ctx->process, process->physProcess + ctx->offsets.threadListHead) - ctx->offsets.threadListEntry;
	uint64_t teb = VMemReadU64(&ctx->process, process->dirBase, ethread + ctx->offsets.teb) + 0x2000;
	uint32_t ppeb;
	VMemRead(&ctx->process, process->dirBase, (uint64_t)&ppeb, teb + ctx->offsets.peb32, sizeof(ppeb));
	VMemRead(&ctx->process, process->dirBase, (uint64_t)&peb, ppeb, sizeof(PEB32));
	return peb;
}


/*
  The low stub (if exists), contains PML4 (kernel DirBase) and KernelEntry point.
  Credits: PCILeech
*/
static int CheckLow(const WinCtx* ctx, uint64_t* pml4, uint64_t* kernelEntry)
{
	int i, o;
	char buf[0x10000];
	for (i = 0; i < 10; i++) {
		MemRead(&ctx->process, (uint64_t)buf, i * 0x10000, 0x10000);
		for (o = 0; o < 0x10000; o += 0x1000) {
			if(0x00000001000600E9 ^ (0xffffffffffff00ff & *(uint64_t*)(void*)(buf + o)))
				continue;
			if(0xfffff80000000000 ^ (0xfffff80000000000 & *(uint64_t*)(void*)(buf + o + 0x70)))
				continue;
			if(0xffffff0000000fff & *(uint64_t*)(void*)(buf + o + 0xa0))
				continue;
			*pml4 = *(uint64_t*)(void*)(buf + o + 0xa0);
			*kernelEntry = *(uint64_t*)(void*)(buf + o + 0x70);
			return 1;
		}
	}
	return 0;
}

static void FindNTKernel(WinCtx* ctx, uint64_t kernelEntry)
{
	uint64_t i, o, p, u, mask = 0xfffff;
	char buf[0x10000];

	ctx->ntKernel = 0;

	while (mask >= 0xfff) {
		for (i = (kernelEntry & ~0x1fffff) + 0x20000000; i > kernelEntry - 0x20000000; i -= 0x200000) {
			for (o = 0; o < 0x20; o++) {
				VMemRead(&ctx->process, ctx->initialProcess.dirBase, (uint64_t)buf, i + 0x10000 * o, 0x10000);
				for (p = 0; p < 0x10000; p += 0x1000) {
					if (((i + 0x1000 * o + p) & mask) == 0 && *(short*)(void*)(buf + p) == IMAGE_DOS_SIGNATURE) {
						int kdbg = 0, poolCode = 0;
						for (u = 0; u < 0x1000; u++) {
							kdbg = kdbg || *(uint64_t*)(void*)(buf + p + u) == 0x4742444b54494e49;
							poolCode = poolCode || *(uint64_t*)(void*)(buf + p + u) == 0x45444f434c4f4f50;
							if (kdbg & poolCode) {
								ctx->ntKernel = i + 0x10000 * o + p;
								if (GenerateExportList(ctx, &ctx->initialProcess, ctx->ntKernel, &ctx->ntExports)) {
									ctx->ntKernel = 0;
									break;
								}
								return;
							}
						}
					}
				}
			}
		}

		mask = mask >> 4;
	}
}

static uint16_t GetNTVersion(const WinCtx* ctx)
{
	uint64_t getVersion = FindProcAddress(ctx->ntExports, "RtlGetVersion");

	if (!getVersion)
		return 0;

	char buf[0x100];
	VMemRead(&ctx->process, ctx->initialProcess.dirBase, (uint64_t)buf, getVersion, 0x100);

	char major = 0, minor = 0;

	/* Find writes to rcx +4 and +8 -- those are our major and minor versions */
	for (char* b = buf; b - buf < 0xf0; b++) {
		if (!major && !minor)
			if (*(uint32_t*)(void*)b == 0x441c748)
				return ((uint16_t)b[4]) * 100 + (b[5] & 0xf);
		if (!major && (*(uint32_t*)(void*)b & 0xfffff) == 0x441c7)
			major = b[3];
		if (!minor && (*(uint32_t*)(void*)b & 0xfffff) == 0x841c7)
			minor = b[3];
	}

	if (minor >= 100)
		minor = 0;

	return ((uint16_t)major) * 100 + minor;
}

static uint32_t GetNTBuild(const WinCtx* ctx)
{
	uint64_t getVersion = FindProcAddress(ctx->ntExports, "RtlGetVersion");

	if (!getVersion)
		return 0;

	char buf[0x100];
	VMemRead(&ctx->process, ctx->initialProcess.dirBase, (uint64_t)buf, getVersion, 0x100);

	/* Find writes to rcx +12 -- that's where the version number is stored. These instructions are not on XP, but that is simply irrelevant. */
	for (char* b = buf; b - buf < 0xf0; b++) {
		uint32_t val = *(uint32_t*)(void*)b & 0xffffff;
		if (val == 0x0c41c7 || val == 0x05c01b)
			return *(uint32_t*)(void*)(b + 3);
	}

	return 0;
}

static int SetupOffsets(WinCtx* ctx)
{
	switch (ctx->ntVersion) {
	  case 502: /* XP SP2 */
		  ctx->offsets = (WinOffsets){
			  .apl = 0xe0,
			  .session = 0x260,
			  .stackCount = 0xa0,
			  .imageFileName = 0x268,
			  .dirBase = 0x28,
			  .peb = 0x2c0,
			  .peb32 = 0x30,
			  .threadListHead = 0x290,
			  .threadListEntry = 0x3d0,
			  .teb = 0xb0
		  };
		  break;
	  case 601: /* W7 */
		  ctx->offsets = (WinOffsets){
			  .apl = 0x188,
			  .session = 0x2d8,
			  .stackCount = 0xdc,
			  .imageFileName = 0x2e0,
			  .dirBase = 0x28,
			  .peb = 0x338,
			  .peb32 = 0x30,
			  .threadListHead = 0x300,
			  .threadListEntry = 0x420, /* 0x428 on later SP1 */
			  .teb = 0xb8
		  };
		  /* SP1 */
		  if (ctx->ntBuild == 7601)
			  ctx->offsets.imageFileName = 0x2d8;
		  break;
	  case 602: /* W8 */
		  ctx->offsets = (WinOffsets){
			  .apl = 0x2e8,
			  .session = 0x430,
			  .stackCount = 0x234,
			  .imageFileName = 0x438,
			  .dirBase = 0x28,
			  .peb = 0x338, /*peb will be wrong on Windows 8 and 8.1*/
			  .peb32 = 0x30,
			  .threadListHead = 0x470,
			  .threadListEntry = 0x400,
			  .teb = 0xf0
		  };
		  break;
	  case 603: /* W8.1 */
		  ctx->offsets = (WinOffsets){
			  .apl = 0x2e8,
			  .session = 0x430,
			  .stackCount = 0x234,
			  .imageFileName = 0x438,
			  .dirBase = 0x28,
			  .peb = 0x338,
			  .peb32 = 0x30,
			  .threadListHead = 0x470,
			  .threadListEntry = 0x688, /* 0x650 on previous builds */
			  .teb = 0xf0
		  };
		  break;
	  case 1000: /* W10 */
		  ctx->offsets = (WinOffsets){
			  .apl = 0x2e8,
			  .session = 0x448,
			  .stackCount = 0x23c,
			  .imageFileName = 0x450,
			  .dirBase = 0x28,
			  .peb = 0x3f8,
			  .peb32 = 0x30,
			  .threadListHead = 0x488,
			  .threadListEntry = 0x6a8,
			  .teb = 0xf0
		  };

		  if (ctx->ntBuild >= 18362) { /* Version 1903 or higher */
			  ctx->offsets.apl = 0x2f0;
			  ctx->offsets.threadListEntry = 0x6b8;
		  }

		  break;
	  default:
		  return 1;
	}
	return 0;
}

static void FillModuleList64(const WinCtx* ctx, const WinProc* process, WinModuleList* list, size_t* maxSize, char* x86)
{
	PEB peb = GetPeb(ctx, process);
	PEB_LDR_DATA ldr;
	memset(&ldr, 0, sizeof(ldr));
	VMemRead(&ctx->process, process->dirBase, (uint64_t)&ldr, peb.Ldr, sizeof(ldr));

	uint64_t head = ldr.InMemoryOrderModuleList.f_link;
	size_t i = list->size ? list->size - 1 : list->size;

	FillAnyModuleList64(ctx, process, list, maxSize, head, 1);

	for (; i < list->size; i++) {
		if (!strcmp(list->list[i].name, "wow64.dll")) {
			*x86 = 1;
			break;
		}
	}
}

static int FillModuleInfo64(const WinCtx* ctx, const WinProc* process, uint64_t* head, int inMemoryOrder, WinModule* modinfo)
{
	const size_t NAME_LEN = 128;
	wchar_t buf[128];

	LDR_MODULE mod;
	memset(&mod, 0, sizeof(mod));

	VMemRead(&ctx->process, process->dirBase, (uint64_t)&mod, *head - sizeof(LIST_ENTRY) * inMemoryOrder, sizeof(mod));
	VMemRead(&ctx->process, process->dirBase, (uint64_t)head, *head, sizeof(*head));

	if (!mod.BaseDllName.length || !mod.SizeOfImage)
		return 1;

	/* Cap the module size */
	if (mod.BaseDllName.length >= NAME_LEN)
		mod.BaseDllName.length = NAME_LEN;

	VMemRead(&ctx->process, process->dirBase, (uint64_t)buf, mod.BaseDllName.buffer, mod.BaseDllName.length * sizeof(wchar_t));
	char* buf2 = (char*)malloc(mod.BaseDllName.length);
	for (int i = 0; i < mod.BaseDllName.length; i++)
		buf2[i] = ((char*)buf)[i*2];
	buf2[mod.BaseDllName.length-1] = '\0';

	if (*(short*)(void*)buf2 == 0x53) { /* 'S\0', a bit of magic, but it works */
		free(buf2);
		return 2;
	}

	modinfo->name = buf2;
	modinfo->baseAddress = mod.BaseAddress;
	modinfo->entryPoint = mod.EntryPoint;
	modinfo->sizeOfModule = mod.SizeOfImage;
	modinfo->loadCount = mod.LoadCount;

	return 0;
}

static WinModule GetBaseModule(const WinCtx* ctx, const WinProc* process)
{
	WinModule mod;
	mod.name = NULL;

	PEB peb = GetPeb(ctx, process);
	PEB_LDR_DATA ldr;
	memset(&ldr, 0, sizeof(ldr));
	VMemRead(&ctx->process, process->dirBase, (uint64_t)&ldr, peb.Ldr, sizeof(ldr));

	uint64_t head = ldr.InMemoryOrderModuleList.f_link;
	uint64_t end = head;
	uint64_t prev = head+1;

	do {
		prev = head;

		if (!FillModuleInfo64(ctx, process, &head, 1, &mod)) {
			if (mod.baseAddress == peb.ImageBaseAddress) {
				break;
			} else {
				free(mod.name);
				mod.name = NULL;
			}
		}
	} while (head != end && head != prev);

	return mod;
}

static void FillAnyModuleList64(const WinCtx* ctx, const WinProc* process, WinModuleList* list, size_t* maxSize, uint64_t head, int inMemoryOrder)
{
	uint64_t end = head;
	uint64_t prev = head+1;

	if (inMemoryOrder)
		inMemoryOrder = 1;

	do {
		if (list->size >= *maxSize) {
			*maxSize *= 2;
			WinModule* newList = (WinModule*)realloc(list->list, sizeof(WinModule) * *maxSize);
			if (!newList)
				break;
			list->list = newList;
		}
		prev = head;

		if (!FillModuleInfo64(ctx, process, &head, inMemoryOrder, list->list + list->size))
			list->size++;

	} while (head != end && head != prev);
}

static void FillModuleList32(const WinCtx* ctx, const WinProc* process, WinModuleList* list, size_t* maxSize)
{
	PEB32 peb = GetPeb32(ctx, process);
	PEB_LDR_DATA32 ldr;
	memset(&ldr, 0, sizeof(ldr));
	VMemRead(&ctx->process, process->dirBase, (uint64_t)&ldr, peb.Ldr, sizeof(ldr));

	uint32_t head = ldr.InMemoryOrderModuleList.f_link;
	uint32_t end = head;
	uint32_t prev = head+1;

	size_t nameBufSize = 128;
	wchar_t* buf = (wchar_t*)malloc(sizeof(wchar_t) * nameBufSize);

	do {
		if (list->size >= *maxSize) {
			*maxSize *= 2;
			WinModule* newList = (WinModule*)realloc(list->list, sizeof(WinModule) * *maxSize);
			if (!newList)
				break;
			list->list = newList;
		}
		prev = head;

		LDR_MODULE32 mod;
		memset(&mod, 0, sizeof(mod));

		VMemRead(&ctx->process, process->dirBase, (uint64_t)&mod, head - sizeof(LIST_ENTRY32), sizeof(mod));
		VMemRead(&ctx->process, process->dirBase, (uint64_t)&head, head, sizeof(head));

		if (!mod.BaseDllName.length || !mod.SizeOfImage)
			continue;

		if (mod.BaseDllName.length >= nameBufSize) {
			nameBufSize = mod.BaseDllName.length * 2;
			buf = (wchar_t*)realloc(buf, sizeof(wchar_t) * nameBufSize);
		}

		VMemRead(&ctx->process, process->dirBase, (uint64_t)buf, mod.BaseDllName.buffer, mod.BaseDllName.length * sizeof(wchar_t));
		char* buf2 = (char*)malloc(mod.BaseDllName.length);
		for (int i = 0; i < mod.BaseDllName.length; i++)
			buf2[i] = ((char*)buf)[i*2];
		buf2[mod.BaseDllName.length-1] = '\0';

		if (*(short*)(void*)buf2 == 0x53) {
			free(buf2);
			continue;
		}

		list->list[list->size].name = buf2;
		list->list[list->size].baseAddress = mod.BaseAddress;
		list->list[list->size].entryPoint = mod.EntryPoint;
		list->list[list->size].sizeOfModule = mod.SizeOfImage;
		list->list[list->size].loadCount = mod.LoadCount;
		list->size++;
	} while (head != end && head != prev);

	free(buf);
}
