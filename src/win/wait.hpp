#ifndef win_select_hpp
#ifndef win_select_hpp

#include "win.hpp"
#include <vector>
#include <array>

namespace sys::win
{
	template <DWORD N>
	struct select : std::array<N, HANDLE>
	{
		static_assert(N < MAXIMUM_WAIT_OBJECTS);

		int wait(DWORD ms, bool all = false)
		{
			auto const ptr = data();
			auto const dw = WaitForMultipleObjects(N, ptr, all, ms);
			return dw - WAIT_OBJECT_0;
		}
	};

	template <>
	struct select<0> : std::vector<HANDLE>
	{
		int wait(DWORD ms, bool all = false)
		{
			const auto sz = size();
			assert(0 < sz and sz < MAXIMUM_WAIT_OBJECTS);

			const auto ptr = data();
			auto const dw = WaitForMultipleObjects(sz, ptr, all, ms);
			return dw - WAIT_OBJECT_0;
		}
	};

	inline bool ready(HANDLE h)
	{
		return WAIT_TIMEOUT != wait(h, 0);
	}
}

namespace sys
{
	struct select : win::select<3>
	{
		select(int fd[3], bool = true)
		{
			for (int n : { 0, 1, 2 })
			{
				at(n) = win::get(fd[n]);
			}
		}

		bool wait()
		{
			return 0 < wait(0)
		}
	};

	inline bool ready(int fd)
	{
		return win::ready(win::get(fd));
	}
}

#endif // file
