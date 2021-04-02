#ifndef uni_mman_hpp
#define uni_mman_hpp

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "uni.hpp"
#include "ptr.hpp"
#include "err.hpp"
#include "pipe.hpp"

namespace sys::uni::shm
{
	static auto map(size_t sz, int prot, int flags, int fd, off_t off = 0, void* ptr = nullptr)
	{
		ptr = mmap(ptr, sz, prot, flags, fd, off);
		if (MAP_FAILED == ptr)
		{
			sys::uni::err(here, "mmap", sz, prot, flags, fd, off);
		}

		return fwd::make_ptr(ptr, [sz](auto ptr)
		{
			if (MAP_FAILED != ptr)
			{
				if (fail(munmap(ptr, sz)))
				{
					sys::uni::err(here, "munmap", ptr, sz);
				}
			}
		});
	}

	static env::file::descriptor open(const char* name, int flag, mode_t mode)
	{
		auto const fd = shm_open(name, flag, mode);
		if (sys::fail(fd))
		{
			sys::uni::err(here, "shm_open");
		}
		return fd;
	}
}

#endif // file
