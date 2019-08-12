// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "cpu.hpp"
#include "err.hpp"

#ifdef _WIN32
# include "win.hpp"
#else
# include "uni.hpp"
#endif

namespace
{
	struct : env::word
	{
		operator long() const final
		{
			#ifdef _WIN32
			{
				sys::win::info const info;
				auto const dw = info.dwNumberOfProcessors;
				return static_cast<long>(dw);
			}
			#else
			{
				#ifdef _SC_NPROCESSORS_ONLN
				{
					return sysconf(_SC_NPROCESSORS_ONLN);
				}
				#else
				{
					sys::warn("Cannot determine CPU count.");
					return sys::invalid;
				}
				#endif
			}
			#endif
		}

	} COUNT;

	struct : env::word
	{
		operator long() const final
		{
			#ifdef _WIN32
			{
				sys::win::info const info;
				auto const dw = info.dwPageSize;
				return static_cast<long>(dw);
			}
			#else
			{
				return sysconf(_SC_PAGE_SIZE);
			}
			#endif 
		}

	} PAGE_SIZE;
}

namespace sys::cpu
{
	::env::word const& count = COUNT;
	::env::word const& page_size = PAGE_SIZE;
}

