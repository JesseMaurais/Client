#include "err.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>

#ifdef _WIN32
# include "win.hpp"
#else
# include "uni.hpp"
#endif

namespace
{
	sys::mutex key;
	bool recursive = false;
}

namespace fmt
{
	std::ostream& operator<<(std::ostream& os, where const& pos)
	{
		return os << pos.file << "(" << pos.line << ") " << pos.func << ":";
	}
}

namespace sys::impl
{
	void warn(fmt::string const& s)
	{
		if (recursive) return;
		scoped_bool scope(recursive);
		auto const unlock = key.lock();
		std::cerr << s << std::endl;
	}

	void err(fmt::string const& s)
	{
		if (recursive) return;
		scoped_bool scope(recursive);
		auto const unlock = key.lock();
		auto const e = std::strerror(errno);
		std::cerr << s << ": " << e << std::endl;
	}
}

