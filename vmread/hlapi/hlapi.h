#ifndef HLAPI_H
#define HLAPI_H

/* A high level C++ wrapper for various memory functions */

#include "../wintools.h"
#include "../mem.h"

#include <stdexcept>
#include <string.h>
#include <vector>
#include <algorithm>

class VMException : public std::exception
{
  public:
	VMException(int status)
	{
		value = status;
	}

	int value;
};

template<typename T>
class WinListIterator
{
  public:
	WinListIterator(T* l)
	{
		list = l;
		count = 0;
	}

	WinListIterator(T* l, size_t c)
	{
		list = l;
		count = c;
	}

	auto& operator*()
	{
		return list->list[count];
	}

	WinListIterator& operator++(int c)
	{
		count += c;
		return *this;
	}

	WinListIterator& operator++()
	{
		return operator++(1);
	}
	WinListIterator& operator--(int c)
	{
		count -= c;
		return *this;
	}

	WinListIterator& operator--()
	{
		return operator--(1);
	}

	bool operator==(WinListIterator& rhs)
	{
		return count == rhs.count && list == rhs.list;
	}

	bool operator!=(WinListIterator& rhs)
	{
		return !operator==(rhs);
	}

  protected:
	size_t count;
  private:
	T* list;
};

class WinExportIteratableList
{
  public:
	using iterator = WinListIterator<WinExportList>;
	iterator begin();
	iterator end();
	size_t getSize();
  private:
	friend class WinListIterator<WinExportList>;
	friend class WinDll;
	class WinDll* windll;

	WinExportList list;
};

class WinDll
{
  public:
	uint64_t GetProcAddress(const char* procName);
	WinDll();
	WinDll(const class WinProcess* p, WinModule& i);
	WinDll(WinDll&& rhs);
	WinDll(WinDll& rhs) = delete;
	~WinDll();

	auto& operator=(WinDll&& rhs)
	{
		info = rhs.info;
		std::swap(exports.list, rhs.exports.list);
		process = rhs.process;
		proc = rhs.proc;
		return *this;
	}

	WinModule info;
	WinExportIteratableList exports;
	const class WinProcess* process;
  private:
	WinProc proc;
	friend class WinExportIteratableList;
	void VerifyExportList();
};

class ModuleIteratableList
{
  public:
	using iterator = WinListIterator<ModuleIteratableList>;
	ModuleIteratableList(bool k = false);
	ModuleIteratableList(class WinProcess* p, bool k = false);
	ModuleIteratableList(ModuleIteratableList&& rhs);
	ModuleIteratableList(ModuleIteratableList& rhs) = delete;
	~ModuleIteratableList();
	ModuleIteratableList& operator=(ModuleIteratableList&& rhs) = default;
	iterator begin();
	iterator end();
	size_t getSize();
	void Verify();
	void InvalidateList();
	WinDll* GetModuleInfo(const char* moduleName);
  private:
	friend class WinListIterator<ModuleIteratableList>;
	friend class WinProcess;
	friend class WinProcessList;
	class WinProcess* process;
	bool kernel;
	WinDll* list;
	size_t size;
};

class WriteList
{
  public:
	WriteList(const WinProcess*);
	~WriteList();
	void Commit();

	template<typename T>
	void Write(uint64_t address, T& value)
	{
		writeList.push_back({(uint64_t)buffer.size(), address, sizeof(T)});
		buffer.reserve(sizeof(T));
		std::copy((char*)&value, (char*)&value + sizeof(T), std::back_inserter(buffer));
	}

	const WinCtx* ctx;
	const WinProc* proc;
  private:
	std::vector<RWInfo> writeList;
	std::vector<char> buffer;
};

class WinProcess
{
  public:
	[[deprecated("Please use ModuleIteratableList::GetModuleInfo")]]
	WinDll* GetModuleInfo(const char* moduleName);
	PEB GetPeb();
	WinProcess();
	WinProcess(const WinProc& p, const WinCtx* c);
	WinProcess(WinProcess&& rhs);
	WinProcess(WinProcess& rhs) = delete;
	void UpdateKernelModuleProcess(const WinProc& p);
	WinProcess& operator=(WinProcess&& rhs) noexcept;

	ssize_t Read(uint64_t address, void* buffer, size_t sz);
	ssize_t Write(uint64_t address, void* buffer, size_t sz);

	void ReadMem(uint64_t address, uint64_t remote, int len)
	{
		VMemRead(&ctx->process, proc.dirBase, remote, address, len);	
	}

	template<typename T>
	T Read(uint64_t address)
	{
		T ret;
		VMemRead(&ctx->process, proc.dirBase, (uint64_t)&ret, address, sizeof(T));
		return ret;
	}

	template<typename T>
	void Write(uint64_t address, const T& value)
	{
		VMemWrite(&ctx->process, proc.dirBase, (uint64_t)&value, address, sizeof(T));
	}

	template<typename T>
	void WriteMem(uint64_t address, T value[], int len)
	{
		VMemWrite(&ctx->process, proc.dirBase, (uint64_t)value, address, len);
	}

	WinProc proc;
	const WinCtx* ctx;
	ModuleIteratableList modules;
  protected:
	friend class ModuleIteratableList;
	friend class WriteList;
};

class WinProcessList
{
  public:
	using iterator = WinListIterator<WinProcessList>;
	void Refresh();
	WinProcess* FindProc(const char* name);
	iterator begin();
	iterator end();
	WinProcessList();
	WinProcessList(const WinCtx* pctx);
	WinProcessList(WinProcessList&& rhs);
	WinProcessList(WinProcessList& rhs) = delete;
	~WinProcessList();

	auto& operator=(WinProcessList rhs)
	{
		std::swap(plist, rhs.plist);
		std::swap(list, rhs.list);
		ctx = rhs.ctx;
		return *this;
	}

	const WinCtx* ctx;
  protected:
	friend iterator;
	WinProcList plist;
	WinProcess* list;
	void FreeProcessList();
};

class SystemModuleList
{
  public:

	ModuleIteratableList& Get(WinProcess* p)
	{
		proc.UpdateKernelModuleProcess(p ? p->proc : proc.ctx->initialProcess);
		return proc.modules;
	}

  private:
	friend class WinContext;
	WinProcess proc;
};

class WinContext
{
  public:

	template<typename T>
	T Read(uint64_t address)
	{
		T ret;
		MemRead(&ctx.process, (uint64_t)&ret, address, sizeof(T));
		return ret;
	}

	template<typename T>
	void Write(uint64_t address, T& value)
	{
		MemWrite(&ctx.process, (uint64_t)&value, address, sizeof(T));
	}

	WinContext(pid_t pid)
	{
		int ret = InitializeContext(&ctx, pid);
		if (ret)
			throw VMException(ret);
		processList = WinProcessList(&ctx);
		systemModuleList.proc.ctx = &ctx;
	}

	~WinContext()
	{
		FreeContext(&ctx);
	}

	WinProcessList processList;
	SystemModuleList systemModuleList;
	WinCtx ctx;
};

#endif
