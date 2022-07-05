#ifndef uni_hpp
#define uni_hpp "POSIX Utility"

#ifdef _WIN32
# error POSIX utility header included with WIN32 macro.
#endif

#include <unistd.h>
#include "err.hpp"
#include "sys.hpp"
#include "ptr.hpp"

namespace sys::uni
{
	const char* strerr(int no);

	template <typename... Args> int err(int no, Args... args)
	{
		return debug ? sys::warn(args..., strerr(no)) : -1;
	}

	inline auto enclose(int fd)
	{
		return fwd::null_shared<int>([fd](auto ptr)
		{
			if (not fail(fd) and fail(close(fd)))
			{
				sys::err(here, "close");
			}
			#ifdef assert
			assert(nullptr == ptr);
			#endif
		});
	}

	struct filed : fwd::pair<int, fwd::shared_ptr<int>>
	{
		filed(int fd) : pair(fd, enclose(fd))
		{ }

		operator int() const
		{
			return first;
		}
	};
}

#endif // file
