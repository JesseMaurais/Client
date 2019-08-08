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

	namespace
	{
		sys::mutex key;
	}

	void impl::warn(fmt::string_view u)
	{
		auto const unlock = key.lock();
		std::cerr << u << std::endl;
	}

	void impl::err(fmt::string_view u)
	{
		auto const unlock = key.lock();
		auto const e = std::strerror(errno);
		std::cerr << u << ": " << e << std::endl;
	}
}

