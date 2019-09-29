#ifndef uni_select_hpp
#define uni_select_hpp

#include "uni/uni.hpp"
#include <sys/select.h>

namespace sys::uni
{
	using set = std::pair<int, fd_set[3]>;

	struct select : set
	{
		select() : first(0)
		{
			for (int n : { 0, 1, 2 })
			{
				FD_ZERO(second + n);
			}
		}

		bool get(int fd, int n)
		{
			return FD_ISSET(fd, second[n]);
		}

		int set(int fd, int flags)
		{
			auto const last = first;
			if (flags)
			{
				first = std::max(first, 1 + fd);
			}
			if (flags & R_OK)
			{
				FD_SET(fd, second + 0);
			}
			if (flags & W_OK)
			{
				FD_SET(fd, second + 1);
			}
			if (flags & X_OK)
			{
				FD_SET(fd, second + 2);
			}
			return last;
		}

		int wait(timeval* ptr = nullptr)
		{
		 	return select(first, second + 0, second + 1, second + 2, ptr);
		}

		int wait(timeval& ref)
		{
			return wait(&ref);
		}

		int wait(long m, long s = 0)
		{
			timeval tv { s, m * 1000 };
			return wait(tv);
		}
	};
}

namespace sys
{
	struct select : uni::select
	{
		select(int fd[3], bool invert = true)
		{
			auto const r = invert ? W_OK : R_OK;
			auto const w = invert ? R_OK : W_OK;
			constexpr auto const x = X_OK;
			set(fd[0], r);
			set(fd[1], w);
			set(fd[2], w);
			set(fd[0], x);
			set(fd[1], x);
			set(fd[2], x);
		}

		bool wait()
		{
			return 0 < wait(0);
		}

	private:

		using uni::select::select;
	};

	inline bool ready(int fd)
	{
		sys::uni::select items;
		items.set(fd, R_OK);
		int const n = items.wait(0);
		alert(fail(n));
		return 0 < n;
	}
}

#endif // file
