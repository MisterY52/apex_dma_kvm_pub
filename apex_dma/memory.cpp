#include "memory.h"
#include <unistd.h>

// Credits: learn_more, stevemk14ebr
size_t findPattern(const PBYTE rangeStart, size_t len, const char *pattern)
{
	size_t l = strlen(pattern);
	PBYTE patt_base = static_cast<PBYTE>(malloc(l >> 1));
	PBYTE msk_base = static_cast<PBYTE>(malloc(l >> 1));
	PBYTE pat = patt_base;
	PBYTE msk = msk_base;
	if (pat && msk)
	{
		l = 0;
		while (*pattern)
		{
			if (*pattern == ' ')
				pattern++;
			if (!*pattern)
				break;
			if (*(PBYTE)pattern == (BYTE)'\?')
			{
				*pat++ = 0;
				*msk++ = '?';
				pattern += ((*(PWORD)pattern == (WORD)'\?\?') ? 2 : 1);
			}
			else
			{
				*pat++ = getByte(pattern);
				*msk++ = 'x';
				pattern += 2;
			}
			l++;
		}
		*msk = 0;
		pat = patt_base;
		msk = msk_base;
		for (size_t n = 0; n < (len - l); ++n)
		{
			if (isMatch(rangeStart + n, patt_base, msk_base))
			{
				free(patt_base);
				free(msk_base);
				return n;
			}
		}
		free(patt_base);
		free(msk_base);
	}
	return -1;
}

uint64_t Memory::get_proc_baseaddr()
{
	return proc.baseaddr;
}

process_status Memory::get_proc_status()
{
	return status;
}

void Memory::check_proc()
{
	if (status == process_status::FOUND_READY || status == process_status::FOUND_NO_ACCESS)
	{
		short c;
		Read<short>(proc.baseaddr, c);

		if (c != 0x5A4D)
		{
			status = process_status::FOUND_NO_ACCESS;
		}
		else
		{
			status = process_status::FOUND_READY;
		}
	}
}

bool kernel_init(Inventory *inv, const char *connector_name)
{
	if (inventory_create_connector(inv, connector_name, "", conn.get()))
	{
		printf("Can't create %s connector\n", connector_name);
		return false;
	}
	else
	{
		printf("%s connector created\n", connector_name);
	}

	kernel = std::make_unique<OsInstance<>>();
	if (inventory_create_os(inv, "win32", "", conn.get(), kernel.get()))
	{
		printf("Unable to initialize kernel using %s connector\n", connector_name);
		connector_drop(conn.get());
		kernel.reset();
		return false;
	}

	return true;
}

bool Memory::testDtbValue(const uint64_t &dtb_val)
{
	proc.hProcess.set_dtb(dtb_val, Address_INVALID);
	check_proc();
	if (status == process_status::FOUND_READY)
	{
		lastCorrectDtbPhysicalAddress = dtb_val;
		return true;
	}

	return false;
}

// https://www.unknowncheats.me/forum/apex-legends/670570-quick-obtain-cr3-check.html
bool Memory::bruteforceDtb(uint64_t dtbStartPhysicalAddr, const uint64_t stepPage)
{
	// eac cr3 always end with 0x-----XX000
	// dtbStartPhysicalAddr should be a multiple of 0x1000
	if ((dtbStartPhysicalAddr & 0xFFF) != 0)
		return false;
	if (dtbStartPhysicalAddr > MAX_PHYADDR)
		return false;

	dtbStartPhysicalAddr -= dtbStartPhysicalAddr % stepPage;
	dtbStartPhysicalAddr += lastCorrectDtbPhysicalAddress % stepPage;

	auto start = std::chrono::high_resolution_clock::now();
	bool result = false;
	uint64_t furtherDistance = GetFurtherDistance(dtbStartPhysicalAddr, 0x0, MAX_PHYADDR);
	size_t maxStep = furtherDistance / stepPage;
	uint64_t guessDtbAddr = 0;

	for (size_t step = 0; step < maxStep; step++)
	{
		// bruteforce dtb from middle
		guessDtbAddr = dtbStartPhysicalAddr + step * stepPage;
		if (guessDtbAddr < MAX_PHYADDR)
		{
			if (testDtbValue(guessDtbAddr))
			{
				result = true;
				break;
			}
		}
		// dont forget the other side
		if (dtbStartPhysicalAddr > step * stepPage)
		{
			guessDtbAddr = dtbStartPhysicalAddr - step * stepPage;
			if (testDtbValue(guessDtbAddr))
			{
				result = true;
				break;
			}
		}
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	printf("[+] bruteforce dtb %s to find dtb:0x%lx, time:%ldms\n", result ? "success" : "failed", result ? guessDtbAddr : 0x0, duration.count());

	// In case we cannot get the dtb through this shortcut method.
	if (result == false && stepPage != 0x1000)
	{
		return bruteforceDtb(dtbStartPhysicalAddr, 0x1000);
	}
	return result;
}

void Memory::open_proc(const char *name)
{
	if (!conn)
	{
		conn = std::make_unique<ConnectorInstance<>>();
		Inventory *inv = inventory_scan();

		printf("Init with qemu connector...\n");
		if (!kernel_init(inv, "qemu"))
		{
			printf("Init with kvm connector...\n");
			if (!kernel_init(inv, "kvm"))
			{
				printf("Quitting\n");
				inventory_free(inv);
				exit(1);
			}
		}

		printf("Kernel initialized: %p\n", kernel.get()->container.instance.instance);
	}

	ProcessInfo info;
	info.dtb2 = Address_INVALID;

	if (kernel.get()->process_info_by_name(name, &info))
	{
		status = process_status::NOT_FOUND;
		lastCorrectDtbPhysicalAddress = 0;
		return;
	}

	close_proc();
	if (kernel.get()->clone().into_process_by_info(info, &proc.hProcess))
	{
		status = process_status::FOUND_NO_ACCESS;
		printf("Error while opening process %s\n", name);
		close_proc();
		return;
	}

	ModuleInfo module_info;
	if (proc.hProcess.module_by_name(name, &module_info))
	{
		status = process_status::FOUND_NO_ACCESS;
		auto base_section = std::make_unique<char[]>(8);
		uint64_t *base_section_value = (uint64_t *)base_section.get();
		CSliceMut<uint8_t> slice(base_section.get(), 8);
		uint32_t EPROCESS_SectionBaseAddress_off = 0x520; // win10 >= 20H1
		kernel.get()->read_raw_into(info.address + EPROCESS_SectionBaseAddress_off, slice);
		proc.baseaddr = *base_section_value;

		if (!bruteforceDtb(0x0, 0x100000))
		{
			close_proc();
			return;
		}
	}
	else
	{
		proc.baseaddr = module_info.base;
	}

	status = process_status::FOUND_READY;
}

void Memory::close_proc()
{
	std::lock_guard<std::mutex> l(m);
	proc.hProcess.~IntoProcessInstance();
	proc.baseaddr = 0;
}

uint64_t Memory::ScanPointer(uint64_t ptr_address, const uint32_t offsets[], int level)
{
	if (!ptr_address)
		return 0;

	uint64_t lvl = ptr_address;

	for (int i = 0; i < level; i++)
	{
		if (!Read<uint64_t>(lvl, lvl) || !lvl)
			return 0;
		lvl += offsets[i];
	}

	return lvl;
}