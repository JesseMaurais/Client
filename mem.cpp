#include "mem.hpp"
#include "err.hpp"
#include "sys.hpp"

namespace sys::file
{
	memory::memory(int fd, ssize_t size, size_t offset)
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

		mem = std::make_unique<sys::mem>();
		address = mem->map(fd, size);
		length = size;
	}

	memory::~memory()
	{
		if (mem and address)
		{
			mem->unmap(address, length);
		}
	}
}
