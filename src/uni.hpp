#ifndef uni_hpp
#define uni_hpp

#ifdef _WIN32
# error POSIX utility header included with Win32 macro.
#endif

#include "sys.hpp"
#include "err.hpp"
#include <unistd.h>

namespace sys::uni
{
	constexpr int invalid = -1;

	inline bool fail(int value)
	{
		return invalid == value;
	}

	template <typename... Args>
	inline void err(int no, Args... args)
	{
		sys::warn(args..., fmt::err(no));
	}
}

#endif // file
