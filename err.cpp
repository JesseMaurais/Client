#include "err.hpp"
#include <cstdio>

#ifdef _WIN32
# include "win.hpp"
#else
# include "uni.hpp"
#endif

namespace
{
	struct lock
	#ifdef _WIN32
	{
		static sys::win::handle mutex;

		lock()
		{
			if (not sys::win::fail(mutex.h))
			{
				auto dw = WaitForSingleObject(mutex.h, INFINITE);
				if (WAIT_FAILED == dw)
				{
					sys::win::err(here, "WaitForSingleObject");
				}
			}
		}

		~lock()
		{
			if (not sys::win::fail(mutex.h))
			{
				if (not ReleaseMutex(mutex.h))
				{
					sys::win::err(here, "ReleaseMutex");
				}
			}
		}
	};
	sys::win::handle lock::mutex = CreateMutex(nullptr, true, nullptr);
	#else
	{
		static sys::uni::mutex mutex;

		lock()
		{
			mutex.lock();
		}

		~lock()
		{
			mutex.unlock(); 
		}
	};
	sys::uni::mutex lock::mutex;
	#endif
}

namespace sys
{
	void warn(fmt::string const& s)
	{
		lock const scoped;
		std::fputs(data(s), stderr);
	}

	void err(fmt::string const& s)
	{
		lock const scoped;
		std::perror(data(s));
	}
}

