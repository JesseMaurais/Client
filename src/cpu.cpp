// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "cpu.hpp"
#include "err.hpp"
#include "pos.hpp"

namespace
{
	struct : env::size
	{
		operator fmt::size_type() const final
		{
			#ifdef _WIN32
			{
				sys::win::info const info;
				return info.dwNumberOfProcessors;
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
					return 0;
				}
				#endif
			}
			#endif
		}

	} COUNT;

	struct : env::size
	{
		operator fmt::size_type() const final
		{
			#ifdef _WIN32
			{
				sys::win::info const info;
				return info.dwPageSize;
			}
			#else
			{
				return sysconf(_SC_PAGE_SIZE);
			}
			#endif 
		}

	} PAGE_SIZE;
}

namespace env::sys::cpu
{
	env::size const& count = COUNT;
	env::size const& page_size = PAGE_SIZE;
}

