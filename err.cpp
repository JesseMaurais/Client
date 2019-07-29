#include "err.hpp"
#include <cstdio>

#ifdef _WIN32
# include "win.hpp"
#else
# include "uni.hpp"
#endif

namespace
{
	sys::mutex key;
}

namespace sys
{
	void warn(fmt::string const& s)
	{
		auto const unlock = key.lock();
		std::fputs(data(s), stderr);
	}

	void err(fmt::string const& s)
	{
		auto const unlock = key.lock();
		std::perror(data(s));
	}
}

