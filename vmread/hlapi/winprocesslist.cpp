#include "hlapi.h"

WinProcessList::iterator WinProcessList::begin()
{
	return iterator(this);
}

WinProcessList::iterator WinProcessList::end()
{
	return iterator(this, plist.size);
}

void WinProcessList::Refresh()
{
	FreeProcessList();
	plist = GenerateProcessList(ctx);
	delete[] list;
	list = new WinProcess[plist.size];
	for (size_t i = 0; i < plist.size; i++)
		list[i] = WinProcess(plist.list[i], ctx);
}

WinProcess* WinProcessList::FindProc(const char* name)
{
	if (!plist.list)
		Refresh();

	for (auto& i : *this)
		if (!strcmp(name, i.proc.name))
			return &i;

	return nullptr;
}

WinProcessList::WinProcessList()
{
	plist.list = nullptr;
	plist.size = 0;
	list = nullptr;
	ctx = nullptr;
}

WinProcessList::WinProcessList(const WinCtx* pctx)
	: WinProcessList()
{
	ctx = pctx;
	Refresh();
}

WinProcessList::WinProcessList(WinProcessList&& rhs)
{
	ctx = rhs.ctx;
	std::swap(plist, rhs.plist);
	std::swap(list, rhs.list);
}

WinProcessList::~WinProcessList()
{
	FreeProcessList();
	delete[] list;
}

void WinProcessList::FreeProcessList()
{
	::FreeProcessList(plist);
	plist.list = nullptr;
}
