#include "hlapi.h"

ModuleIteratableList::ModuleIteratableList(bool k)
	: process(nullptr), kernel(k), list(nullptr), size(0)
{
}

ModuleIteratableList::ModuleIteratableList(WinProcess* p, bool k)
	: ModuleIteratableList(k)
{
	process = p;
}

ModuleIteratableList::ModuleIteratableList(ModuleIteratableList&& rhs)
	: process(rhs.process), kernel(rhs.kernel), list(rhs.list), size(rhs.size)
{
	if (process->modules.process != process)
		*(volatile bool*)nullptr = 0;
}

ModuleIteratableList::~ModuleIteratableList()
{
	InvalidateList();
}

ModuleIteratableList::iterator ModuleIteratableList::begin()
{
	Verify();
	return iterator(this);
}

ModuleIteratableList::iterator ModuleIteratableList::end()
{
	Verify();
	return iterator(this, size);
}

size_t ModuleIteratableList::getSize()
{
	Verify();
	return size;
}

void ModuleIteratableList::Verify()
{
	if (!list) {
		WinModuleList ls = !kernel ? GenerateModuleList(process->ctx, &process->proc) : GenerateKernelModuleList(process->ctx);
		list = new WinDll[ls.size];
		size = ls.size;
		for (size_t i = 0; i < size; i++)
			list[i] = WinDll(process, ls.list[i]);
		free(ls.list);
	}
}

void ModuleIteratableList::InvalidateList()
{
	if (list)
		for (size_t i = 0; i < size; i++)
			free(list[i].info.name);
	delete[] list;
	list = nullptr;
}

WinDll* ModuleIteratableList::GetModuleInfo(const char* moduleName)
{
	Verify();
	for (size_t i = 0; i < size; i++)
		if (!strcmp(moduleName, list[i].info.name))
			return list + i;
	return nullptr;
}

WriteList::WriteList(const WinProcess* p)
{
	ctx = p->ctx;
	proc = &p->proc;
}

WriteList::~WriteList()
{
	size_t sz = writeList.size();
	for (size_t i = 0; i < sz; i++)
		free((void*)writeList[i].local);
}

void WriteList::Commit()
{
	size_t sz = writeList.size();

	RWInfo* infos = writeList.data();

	uint64_t bufmem = (uint64_t)buffer.data();

	for (size_t i = 0; i < sz; i++)
		infos[i].local += bufmem;

	VMemWriteMul(&ctx->process, proc->dirBase, infos, sz);

	writeList.clear();
	buffer.clear();
}

WinDll* WinProcess::GetModuleInfo(const char* moduleName)
{
	return modules.GetModuleInfo(moduleName);
}

PEB WinProcess::GetPeb()
{
	return ::GetPeb(ctx, &proc);
}

WinProcess::WinProcess()
	: ctx(nullptr), modules(this)
{
}

WinProcess::WinProcess(const WinProc& p, const WinCtx* c)
	: proc(p), ctx(c), modules(this)
{
}

WinProcess::WinProcess(WinProcess&& rhs)
{
	*this = std::move(rhs);
}

WinProcess& WinProcess::operator=(WinProcess&& rhs) noexcept
{
	proc = rhs.proc;
	ctx = rhs.ctx;
	modules = std::move(rhs.modules);
	modules.process = this;
	return *this;
}

void WinProcess::UpdateKernelModuleProcess(const WinProc& p)
{
	proc = p;
	modules.kernel = true;
}

ssize_t WinProcess::Read(uint64_t address, void* buffer, size_t sz)
{
	return VMemRead(&ctx->process, proc.dirBase, (uint64_t)buffer, address, sz);
}

ssize_t WinProcess::Write(uint64_t address, void* buffer, size_t sz)
{
	return VMemWrite(&ctx->process, proc.dirBase, (uint64_t)buffer, address, sz);
}

