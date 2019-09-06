#ifndef uni_mman_hpp
#define uni_mman_hpp

#include "uni.hpp"
#include "ptr.hpp"
#include "err.hpp"
#include "str.hpp"

namespace sys::uni
{
	extern "C"
	{
		#include <sys/mman.h>
	}

	template <typename C>
	auto make_mmap(size_t sz, int prot, int flags, int fd, off_t off = 0, C* ptr = nullptr)
	{
		ptr = mmap(ptr, sz, prot, flags, fd, off);
		if (MAP_FAILED == ptr)
		{
			sys::err(here, "mmap", ptr, sz, off);
			ptr = nullptr;
		}

		return make_ptr(ptr, [sz](auto ptr)
		{
			if (nullptr != ptr)
			{
				if (fail(munmap(ptr, sz)))
				{
					sys::err(here, "munmap", ptr, sz);
				}
			}
		});
	}

	class shm
	{
		char const *str;

	public:

		int fd;

		shm(char const *name, int flag, mode_t mode) : str(name)
		{
			fd = shm_open(str, flag, mode);
			if (fail(fd))
			{
				err(here, "shm_open", str, flag, mode);
			}
		}

		~shm()
		{
			if (not fail(fd))
			{
				if (fail(shm_unlink(str)))
				{
					err(here, "shm_unlink", str, flag, mode);
				}
			}
		}
	};
}

#endif // file
