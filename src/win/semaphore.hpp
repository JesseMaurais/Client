#ifndef win_base_hpp
#define win_base_hpp

#include "win.hpp"

namespace sys::win
{
	struct semaphore : handle
	{
		semaphore(long most, long init = 0, char const* name = nullptr)
		{
			security
			h = CreateSemaphore(security, init, most, name);
			if (sys::win::fail(h))
			{
				sys::win::err(here, init, most, name);
			}
		}

		long release(long n = 1)
		{
			if (not ReleaseSemaphore(h, n, &n))
			{
				sys::win::err(here, n);
				n = -1;
			}
			return n;
		}
	};
}

namespace sys
{
	struct semaphore : win::semaphore
	{
		using base = win::semaphore;

		sempahore(unsigned most) : base(most)
		{ 
			assert(0 < most);
		}

		auto wait()
		{
			class post
			{
				base* ptr;

			public:

				post(base* that) : ptr(that)
				{
					if (fail(win::wait(ptr->h)))
					{
						ptr = nullptr;
					}
				}

				~post()
				{
					if (nullptr != ptr)
					{
						ptr->release();
					}
				}
			};
			return post(this);
		}
	};
}

#endif // file
