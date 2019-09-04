#ifndef uni_hpp
#define uni_hpp

#ifdef _WIN32
# error POSIX utility header included with Win32 macro.
#endif

#include "sys.hpp"
#include "err.hpp"

namespace sys::uni
{
	extern "C"
	{
		#include <unistd.h>
	}

	template <typename... Args>
	inline void err(int no, Args... args)
	{
		sys::warn(args..., fmt::err(no));
	}
}

#endif // file
