// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "shm.hpp"
#include "sys.hpp"
#include "err.hpp"

#ifdef _WIN32
# include "win.hpp"
# pragma comment(lib, "user32.lib")
#else
# include <sys/mman.h>
#endif

namespace sys::file
{
	void open(int fd, ssize_t size, size_t off, int mode, int type)
	{
		if (size < 0)
		{
			sys::stat_t st;
			if (sys::fail(sys::fstat(fd, &st)))
			{
				sys::perror("fstat");
				return;
			}
			size = st.st_size;
		}

		#ifdef _WIN32
		{
			DWORD flags = 0;
			DWORD prot = 0;

			if (mode & execute)
			{
				flags |= FILE_MAP_EXECUTE;

				if (mode & write)
				{
					prot = PAGE_EXECUTE_READWRITE;
					flags |= FILE_MAP_WRITE;
				}
				else
				{
					prot = PAGE_EXECUTE_READ;
				}

				if (mode & read)
				{
					flags |= FILE_MAP_READ;
				}
			}
			else
			{
				if (mode & write)
				{
					prot = PAGE_READWRITE;
					flags |= FILE_MAP_WRITE;
				}
				else
				{
					prot = PAGE_READONLY;
				}

				if (mode & read)
				{
					flags |= FILE_MAP_READ;
				}
			}

			auto const lo = LOWORD(off);
			auto const hi = HIWORD(off);
			
			sys::win::handle const h = CreateFileMapping
			(
				sys::win::get(fd),
				nullptr,
				prot,
				hi,
				lo,
				nullptr
			);

			if (nullptr == h)
			{
				sys::winerr("CreateFileMapping");
				return;
			}

			address = MapViewOfFile(h, flags, hi, lo, size);
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

			address = mmap(nullptr, size, prot, flags, fd, off);
			if (MAP_FAILED == address)
			{
				sys::perror("mmap");
				address = nullptr;
				return;
			}
		}
		#endif

		length = size;
	}

	void memory::close()
	{
		if (nullptr != address)
		{
			#ifdef _WIN32
			{
				if (not UnmapViewOfFile(address))
				{
					sys::win:perror("UnmapViewOfFile", path);
				}
			}
			#else
			{
				if (sys::fail(munmap(address, length)))
				{
					sys::perror("munmap", path);
				}

				if (not empty(path))
				{
					auto const s = path.c_str();
					if (sys::fail(shm_unlink(s)))
					{
						sys::perror("shm_unlink", path);
					}
				}
			}
			#endif
		}
	}
}
