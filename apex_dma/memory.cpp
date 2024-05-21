#include "memory.h"

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
	if (status == process_status::FOUND_READY)
	{
		short c;
		Read<short>(proc.baseaddr, c);

		if (c != 0x5A4D)
		{
			status = process_status::FOUND_NO_ACCESS;
			close_proc();
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
		return false;
	}

	return true;
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

	if (kernel.get()->process_info_by_name(name, &info))
	{
		status = process_status::NOT_FOUND;
		return;
	}

	if (kernel.get()->process_by_info(info, &proc.hProcess))
	{
		status = process_status::NOT_FOUND;
		return;
	}

	ModuleInfo module_info;

	if (proc.hProcess.module_by_name(name, &module_info))
	{
		printf("Can't find base module info for process %s. Trying with a new dtb...\n", name);

		for (size_t dtb = 0; dtb <= SIZE_MAX; dtb += 0x1000)
		{
			info.dtb1 = dtb;
			info.dtb2 = Address_INVALID;
			kernel.get()->process_by_info(info, &proc.hProcess);

			if (!proc.hProcess.module_by_name(name, &module_info))
				break;

			if (dtb == SIZE_MAX)
			{
				printf("Access error for process %s\n", name);
				status = process_status::FOUND_NO_ACCESS;
				return;
			}
		}
	}

	proc.baseaddr = module_info.base;
	status = process_status::FOUND_READY;
}

void Memory::close_proc()
{
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