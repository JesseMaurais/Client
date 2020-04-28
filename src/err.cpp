#include "err.hpp"
#include "sync.hpp"
#include <system_error>
#include <exception>
#include <iostream>
#include <cstring>

fmt::string::out fmt::operator<<(string::out out, where const& at)
{
	return out << at.file << "(" << at.line << ") " << at.func << ":";
}

fmt::string::out fmt::operator<<(string::out out, std::errc const& errc)
{
	return out << std::make_error_condition(errc).message();
}

fmt::string::out fmt::operator<<(string::out out, std::exception const& ex)
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

	void impl::warn(fmt::string::view u)
	{
		auto const unlock = key.lock();
		std::cerr << u << fmt::eol;
	}

	void impl::err(fmt::string::view u)
	{
		auto const unlock = key.lock();
		auto const e = std::strerror(errno);
		std::cerr << u << ": " << e << fmt::eol;
	}
}

