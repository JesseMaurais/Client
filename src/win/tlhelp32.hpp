#ifndef win_tlhelp32_hpp
#define win_tlhelp32_hpp

#include "win.hpp"
#include "sys.hpp"
#include "ptr.hpp"
#include "err.hpp"
#include "str.hpp"

namespace sys::win
{
	extern "C"
	{
		#include <tlhelp32.h>
	}

	struct snapshot : handle
	{
		snapshot(DWORD dw)
		{
			h = CreateToolhelp32Snapshot(dw, 0);
			if (sys::win::fail(h))
			{
				sys::win::err(here, dw);
			}
		}
	};

	struct processes : size<PROCESSENTRY32>
	{
		snapshot snap;

		processes() : snap(TH32CS_SNAPPROCESS)
		{
			if (not sys::win::fail(snap.h))
			{
				if (not Process32First(snap.h, this))
				{
					sys::win::err(here);
				}
			}
		}

		bool operator++()
		{
			return Process32Next(snap.h, this);
		}
	};

	struct modules : size<MODULEENTRY32>
	{
		snapshot snap;

		modules() : snap(TH32CS_SNAPMODULE)
		{
			if (not sys::win::fail(snap.h))
			{
				if (not Module32First(snap.h, this))
				{
					sys::win::err(here);
				}
			}
		}

		bool operator++()
		{
			return Module32Next(snap.h, this);
		}
	};
}

#endif // file
