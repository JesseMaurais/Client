#ifndef win_base_hpp
#define win_base_hpp

#include "win.hpp"

namespace sys::win
{
	struct semaphore : handle
	{
		semaphore(char const* name = nullptr, long init = 0, long most = 1)
		{
			h = CreateSemaphore(nullptr, init, most, name);
			if (sys::win::fail(h))
			{
				sys::win::err(here, init, most, name);
			}
		}
	};
}

#endif // file
