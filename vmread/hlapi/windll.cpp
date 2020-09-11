#include "hlapi.h"

WinExportIteratableList::iterator WinExportIteratableList::begin()
{
	windll->VerifyExportList();
	return iterator(&list);
}

WinExportIteratableList::iterator WinExportIteratableList::end()
{
	windll->VerifyExportList();
	return iterator(&list, list.size);
}

size_t WinExportIteratableList::getSize()
{
	windll->VerifyExportList();
	return list.size;
}

uint64_t WinDll::GetProcAddress(const char* procName)
{
	VerifyExportList();
	return ::FindProcAddress(exports.list, procName);
}

WinDll::WinDll()
{
	process = nullptr;
	exports.list.list = nullptr;
	exports.list.size = 0;
	exports.windll = this;
}

WinDll::WinDll(const WinProcess* p, WinModule& i)
	: WinDll()
{
	process = p;
	info = i;
}

WinDll::WinDll(WinDll&& rhs)
{
	info = rhs.info;
	process = rhs.process;
	proc = rhs.proc;
	exports = rhs.exports;
	exports.windll = this;
	rhs.exports.list.list = nullptr;
	rhs.exports.list.size = 0;
}

WinDll::~WinDll()
{
	FreeExportList(exports.list);
}

void WinDll::VerifyExportList()
{
	if (proc.dirBase != process->proc.dirBase) {
		proc = process->proc;
		FreeExportList(exports.list);
		memset(&exports.list, 0, sizeof(exports.list));
	}

	if (!exports.list.list)
		GenerateExportList(process->ctx, &proc, info.baseAddress, &exports.list);
}
