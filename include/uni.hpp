#ifndef uni_hpp
#define uni_hpp "POSIX Utility"

#ifdef _WIN32
# error POSIX utility header included with WIN32 macro.
#endif

#include <unistd.h>
#include "err.hpp"
#include "sys.hpp"

namespace sys::uni
{
	const char* strerr(int no);

	template <typename... Args> int err(int no, Args... args)
	{
		return debug ? sys::warn(args..., strerr(no)) : -1;
	}
}

#endif // file
