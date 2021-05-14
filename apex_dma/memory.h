#include "memflow_win32.h"
#include <cstring>
#include <stdio.h>
#include <mutex>

#define INRANGE(x,a,b)		(x >= a && x <= b) 
#define getBits( x )		(INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))
#define getByte( x )		(getBits(x[0]) << 4 | getBits(x[1]))

typedef uint8_t* PBYTE;
typedef uint8_t BYTE;
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef WORD *PWORD;

static CloneablePhysicalMemoryObj *conn = 0;
static Kernel *kernel = 0;

inline bool isMatch(const PBYTE addr, const PBYTE pat, const PBYTE msk)
{
	size_t n = 0;
	while (addr[n] == pat[n] || msk[n] == (BYTE)'?')
	{
		if (!msk[++n])
		{
			return true;
		}
	}
	return false;
}

size_t findPattern(const PBYTE rangeStart, size_t len, const char* pattern);

typedef struct Process
{
	Win32Process* hProcess = 0;
	uint64_t baseaddr = 0;
}Process;

enum class process_status : BYTE
{
	NOT_FOUND,
	FOUND_NO_ACCESS,
	FOUND_READY
};

class Memory
{
private:
	Process proc;
	VirtualMemoryObj* mem;
	process_status status = process_status::NOT_FOUND;
	std::mutex m;
public:
	~Memory() { if (mem) virt_free(mem); if (proc.hProcess) process_free(proc.hProcess); }

	uint64_t get_proc_baseaddr();

	process_status get_proc_status();

	void check_proc();

	void open_proc(const char* name);

	void close_proc();

	template<typename T>
	bool Read(uint64_t address, T& out);

	template<typename T>
	bool ReadArray(uint64_t address, T out[], size_t len);

	template<typename T>
	bool Write(uint64_t address, const T& value);

	template<typename T>
	bool WriteArray(uint64_t address, const T value[], size_t len);

	uint64_t ScanPointer(uint64_t ptr_address, const uint32_t offsets[], int level);
};

template<typename T>
inline bool Memory::Read(uint64_t address, T& out)
{
	std::lock_guard<std::mutex> l(m);
	return mem && virt_read_raw_into(mem, address, (uint8_t*)&out, sizeof(T)) == 0;
}

template<typename T>
inline bool Memory::ReadArray(uint64_t address, T out[], size_t len)
{
	std::lock_guard<std::mutex> l(m);
	return mem && virt_read_raw_into(mem, address, (uint8_t*)out, sizeof(T) * len) == 0;
}

template<typename T>
inline bool Memory::Write(uint64_t address, const T& value)
{
	std::lock_guard<std::mutex> l(m);
	return mem && virt_write_raw(mem, address, (uint8_t*)&value, sizeof(T)) == 0;
}

template<typename T>
inline bool Memory::WriteArray(uint64_t address, const T value[], size_t len)
{
	std::lock_guard<std::mutex> l(m);
	return mem && virt_write_raw(mem, address, (uint8_t*)value, sizeof(T) * len) == 0;
}