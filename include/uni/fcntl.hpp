#ifndef uni_fcntl_hpp
#define uni_fcntl_hpp "POSIX File Control"

#include "uni.hpp"
#include "ptr.hpp"
#include <fcntl.h>

namespace sys::uni::file
{
	struct lock : fwd::zero<flock>
	{
		bool get(int fd)
		{
			return fail(fcntl(fd, F_GETLK, this)) and err(here);
		}

		bool set(int fd)
		{
			return fail(fcntl(fd, F_SETLK, this)) and err(here);
		}

		bool wait(int fd)
		{
			return fail(fctrl(fd, F_SETLKW, this)) and err(here);
		}
	};
}

#endif // file 