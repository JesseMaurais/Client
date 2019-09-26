#include "err.hpp"
#include "api.hpp"
#include "ptr.hpp"
#include "thread.hpp"
#include <system_error>
#include <exception>
#include <iostream>
#include <cstring>

std::ostream& fmt::operator<<(std::ostream& out, fmt::where const& pos)
{
	return out << pos.file << "(" << pos.line << ") " << pos.func << ":";
}

std::ostream& fmt::operator<<(std::ostream& out, std::errc const& errc)
{
	return out << std::make_error_condition(errc).message();
}

std::ostream& fmt::operator<<(std::ostream& out, std::exception const& ex)
{
	return out << ex.what();
}

namespace sys
{
	bool debug =
	#ifdef NDEBUG
		false;
	#else
		true;
	#endif

	bool iserr(std::errc ec)
	{
		auto const code = std::make_error_code(ec);
		return code.value() == errno;
	}

	namespace
	{
		sys::mutex key;
	}

	void impl::warn(fmt::view u)
	{
		auto const unlock = key.lock();
		std::cerr << u << fmt::eol;
	}

	void impl::err(fmt::view u)
	{
		auto const unlock = key.lock();
		auto const e = std::strerror(errno);
		std::cerr << u << ": " << e << fmt::eol;
	}
}

