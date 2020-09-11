#ifndef POINTERS_H
#define POINTERS_H
#include "hlapi.h"

template<typename T, WinProcess*& P>
struct vptr
{
	uint64_t addr;
	T value;

	vptr(uint64_t address)
	{
		Init(address);
	}

	inline void Init(uint64_t address)
	{
		addr = address;
	}

	inline T& operator* () {
		value = P->Read<T>(addr);
		return value;
	}
};

#endif
