#include "env.hpp"
#include "dbg.hpp"
#include "sys.hpp"
#include "os.hpp"

namespace sys::env
{
	std::string_view get(std::string_view s)
	{
		return std::getenv(s.data());
	}

	bool put(std::string_view s)
	{
		return sys::putenv(s.data()) != 0;
	}
}

namespace sys
{
	std::span<std::string_view> environment()
	{
		static std::vector<std::string_view> span;
		span.clear();
		if (sys::environ)
		{
			for (auto var = sys::environ; *var; ++var)
			{
				span.push_back(*var);
			}
		}
		return span;
	}
}

namespace env
{
	std::string_view user()
	{
		if constexpr (sys::POSIX)
		{
			return sys::env::get("USER");
		}
		else
		if constexpr (sys::WIN32)
		{
			return sys::env::get("USERNAME");
		}
	}

	std::string_view home()
	{
		if constexpr (sys::POSIX)
		{
			return sys::env::get("HOME");
		}
		else
		if constexpr (sys::WIN32)
		{
			return sys::env::get("USERPROFILE");
		}
	}

	std::string_view temp()
	{
		for (auto var : { "TEMP", "TMP", "TEMPDIR" })
		{
			auto view = sys::env::get(var);
			if (not view.empty())
			{
				return view;
			}
		}
		return "";
	}

	std::string_view shell()
	{
		if constexpr (sys::POSIX)
		{
			return sys::env::get("SHELL");
		}
		else
		if constexpr (sys::WIN32)
		{
			return sys::env::get("COMSPEC");
		}
	}
}
