// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "mem.hpp"
#include "err.hpp"
#include "sys.hpp"

#ifdef _WIN32
# include "win.hpp"
#else
# include <sys/mman.h>
#endif

namespace sys::file
{
	memory::memory(int fd, ssize_t size, size_t off, int mode, int type)
	{
		if (size < 0)
		{
			sys::stat_t st;
			if (sys::fail(sys::fstat(fd, &st)))
			{
				sys::perror("fstat");
				return;
			}
			length = st.st_size;
		}
		else length = size;

		#ifdef _WIN32
		{
			DWORD access = 0;
			DWORD protect = 0;

			if (mode & execute)
			{
				access |= FILE_MAP_EXECUTE;

				if (mode & write)
				{
					protect = PAGE_EXECUTE_READWRITE;
					access |= FILE_MAP_WRITE;
				}
				else
				{
					protect = PAGE_EXECUTE_READ;
				}

				if (mode & read)
				{
					access |= FILE_MAP_READ;
				}
			}
			else
			{
				if (mode & write)
				{
					protect = PAGE_READWRITE;
					access |= FILE_MAP_WRITE;
				}
				else
				{
					protect = PAGE_READONLY;
				}

				if (mode & read)
				{
					access |= FILE_MAP_READ;
				}
			}

			auto const lo = LOWORD(off);
			auto const hi = HIWORD(off);
			
			sys::win::handle const h = CreateFileMapping
			(
				sys::win::get(fd),
				nullptr,
				protect,
				hi,
				lo,
				nullptr
			);

			if (nullptr == h)
			{
				sys::winerr("CreateFileMapping");
				return;
			}

			address = MapViewOfFile(h, access, hi, lo, length);
			if (nullptr == address)
			{
				sys::winerr("MapViewOfFile");
				return;
			}
		}
		#else
		{
			int prot = 0;
			if (mode & read) prot |= PROT_READ;
			if (mode & write) prot |= PROT_WRITE;
			if (mode & execute) prot |= PROT_EXEC;

			int flags = 0;
			if (type & share) flags |= MAP_SHARED;
			if (type & privy) flags |= MAP_PRIVATE;
			if (type & fixed) flags |= MAP_FIXED;

			address = mmap(nullptr, length, prot, flags, fd, off);
			if (MAP_FAILED == address)
			{
				sys::perror("mmap");
				address = nullptr;
				return;
			}
		}
		#endif
	}

	memory::~memory()
	{
		if (nullptr != address)
		{
			#ifdef _WIN32
			{
				if (not UnmapViewOfFile(address))
				{
					sys::winerr("UnmapViewOfFile");
				}
			}
			#else
			{
				if (sys::fail(munmap(address, length)))
				{
					sys::perror("munmap");
				}
			}
			#endif
		}
	}
}
