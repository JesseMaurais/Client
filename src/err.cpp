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

namespace
{
	auto thread_id()
	{
		
	}

	struct : env::variable<fmt::string::view>
	{
		thread_local type id = thread_id();

		operator type() const final
		{
			return id;
		}

		type operator=()(type value) final
		{
			return id = value;
		}

	} THREAD_ID;
}

namespace sys
{
	env::view:ref thread_id = THREAD_ID;

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

	int bug(fmt::string::view u, bool no)
	{
		thread_local struct
		{
			fmt::string s;
			int n = -1;
		} local;
		// Avoid spamming
		if (u != local.s)
		{
			// reset
			local.n = 0;
			local.s = fmt::to_string(u);
			// sync
			static sys::mutex key;
			auto const unlock = key.lock();
			// format
			std::cerr << u;
			{
				// number
				if (not no)
				{
					const auto e = std::strerror(errno);
					std::cerr << ": " << e;
				}
				// thread
				view const id = thread_id;
				if (not empty(id))
				{
					std::cerr << '[' << id << ']';
				}
			}
			std::cerr << '\n';
		}
		else ++local.n;
		return local.n;
	}

	int impl::warn(fmt::string::view u)
	{
		return bug(u, true);
	}

	int impl::err(fmt::string::view u)
	{
		return bug(u, false);
	}
}

