// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "mem.hpp"
#include "err.hpp"
#include "sys.hpp"
#include "int.hpp"

#ifdef _WIN32
# include "mman.c"
#else
# include <sys/mman.h>
#endif

namespace sys::file
{
	memory::memory(int fd, ssize_t size, size_t offset, int mode, int type)
	{
		if (size < 0)
		{
			sys::stat_t st;
			if (fail(sys::fstat(fd, &st)))
			{
				sys::perror("fstat");
				return;
			}
			size = st.st_size;
		}

		int prot = 0;
		if (mode & read) prot |= PROT_READ;
		if (mode & write) prot |= PROT_WRITE;
		if (mode & execute) prot |= PROT_EXEC;

		int flags = 0;
		if (type & share) flags |= MAP_SHARED;
		if (type & privy) flags |= MAP_PRIVATE;
		if (type & fixed) flags |= MAP_FIXED;

		address = mmap(nullptr, size, prot, flags, fd, offset);
		if (MAP_FAILED == address)
		{
			sys::perror("mmap");
			address = nullptr;
			return;
		}
		length = size;
	}

	memory::~memory()
	{
		if (nullptr != address)
		{
			if (fail(munmap(address, length)))
			{
				sys::perror("munmap");
			}
		}
	}
}
