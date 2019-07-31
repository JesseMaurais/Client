#include "err.hpp"
#include "ptr.hpp"
#include <iostream>
#include <cstring>
#include <cerrno>

#ifdef _WIN32
# include "win.hpp"
#else
# include "uni.hpp"
#endif

namespace
{
	sys::mutex key;
	int recursive = 0;
}

std::ostream& fmt::operator<<(std::ostream& os, fmt::where const& pos)
{
	return os << pos.file << "(" << pos.line << ") " << pos.func << ":";
}

namespace sys
{
	bool debug =
	#ifdef NDEBUG
		false;
	#else
		true;
	#endif

	void impl::warn(fmt::string_view u)
	{
		if (recursive) return;
		etc::counter scope(recursive);
		auto const unlock = key.lock();
		std::cerr << u << std::endl;
	}

	void impl::err(fmt::string_view u)
	{
		if (recursive) return;
		etc::counter scope(recursive);
		auto const unlock = key.lock();
		auto const e = std::strerror(errno);
		std::cerr << u << ": " << e << std::endl;
	}
}

