#ifndef uni_mman_hpp
#define uni_mman_hpp "POSIX Shared Memory"

#include "uni.hpp"
#include "ptr.hpp"
#include "err.hpp"
#include "pipe.hpp"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace sys::uni::shm
{
	template <class Type> auto make_ptr(Type* ptr, size_t sz)
	{
		return fwd::make_ptr(ptr, [sz](auto ptr)
		{
			if (MAP_FAILED != ptr)
			{
				if (fail(munmap(ptr, sz)))
				{
					sys::err(here, "munmap", ptr, sz);
				}
			}
		});
	}

	template <class Type = void> auto map(size_t sz, int prot, int flags, int fd, off_t off = 0, Type* ptr = nullptr)
	{
		ptr = mmap(ptr, sz, prot, flags, fd, off);
		if (MAP_FAILED == ptr)
		{
			sys::err(here, "mmap", sz, prot, flags, fd, off);
		}
		return make_ptr(ptr, sz);
	}

	inline auto open(const char* name, int flag, mode_t mode)
	{
		auto const fd = shm_open(name, flag, mode);
		if (sys::fail(fd))
		{
			sys::err(here, "shm_open");
		}
		return env::file::descriptor(fd);
	}
}

#endif // file
