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
		return fmt::debug ? fmt::warning(args..., strerr(no)) : -1;
	}

	inline auto enclose(int fd)
	{
		return fwd::shared_ptr<void>(nullptr, [fd](auto ptr) noexcept
		{
			if (not fail(fd) and fail(close(fd)))
			{
				perror("close");
			}
			#ifdef assert
			assert(nullptr == ptr);
			#endif
		});
	}

	struct filed : fwd::source<int>
	{
		filed(int fd) : source(fd, enclose(fd)) { }
	};
}

#endif // file
