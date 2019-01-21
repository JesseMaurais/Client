#include "mem.hpp"
#include "err.hpp"
#include "sys.hpp"
#include "int.hpp"

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
		length = fmt::to_size(size);
		address = mem->map(fd, length, offset);
	}

	memory::~memory()
	{
		if (mem and address)
		{
			mem->unmap(address, length);
		}
	}
}
