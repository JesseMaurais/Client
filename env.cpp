// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "env.hpp"
#include "fmt.hpp"
#include "sys.hpp"
#include "dbg.hpp"
#include <regex>

namespace
{
	fmt::string_view eval(std::string_view u)
	{
		auto const i = u.find_first_not_of(sys::esc::sh::first);
		auto const j = u.find_last_of(sys::esc::sh::second);
		auto const v = u.substr(i, j - i);
		return sys::env::get(v);
	}
}

namespace sys::env
{
	fmt::string_view get(fmt::string_view u)
	{
		assert(fmt::terminated(u));
		auto const p = u.data();
		return std::getenv(p);
	}

	bool put(fmt::string_view u)
	{
		assert(fmt::terminated(u));
		auto p = const_cast<char*>(u.data());
		return 0 != sys::putenv(p);
	}

	bool set(fmt::string_view u, fmt::string_view v)
	{
		auto const s = fmt::key_value(u, v);
		return put(s);
	}

	fmt::string eval(fmt::string u)
	{
		static std::regex x { sys::esc::sh::regex };
		std::smatch m;
		fmt::string s;
		while (std::regex_search(u, m, x))
		{
			s += m.prefix();
			auto t = m.str();
			auto v = ::eval(t);
			s.append(v.data(), v.size());
			u = m.suffix();
		}
		s.shrink_to_fit();
		return s;
	}
}

namespace
{
	struct : env::list
	{
		operator fmt::span_view() const final
		{
			fmt::string_view u = sys::env::get("PATH");
			return fmt::split(u, sys::sep::path);
		}

	} PATH;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			if constexpr (sys::posix)
			{
				return sys::env::get("USER");
			}
			else
			if constexpr (sys::win32)
			{
				return sys::env::get("USERNAME");
			}
		}

	} USER;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			if constexpr (sys::posix)
			{
				return sys::env::get("HOME");
			}
			else
			if constexpr (sys::win32)
			{
				return sys::env::get("USERPROFILE");
			}
		}

	} HOME;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			if constexpr (sys::posix)
			{
				return sys::env::get("PWD");
			}
			else
			if constexpr (sys::win32)
			{
				return sys::env::get("CD");
			}
		}

	} PWD;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			for (auto var : { "LC_ALL", "LC_MESSAGES", "LANG" })
			{
				auto view = sys::env::get(var);
				if (not empty(view))
				{
					return view;
				}
			}
			return "";
		}

	} LANG;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			for (auto var : { "TMPDIR", "TEMP", "TMP" })
			{
				auto view = sys::env::get(var);
				if (not empty(view))
				{
					return view;
				}
			}
			return "";
		}

	} TMPDIR;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			if constexpr (sys::posix)
			{
				return sys::env::get("SHELL");
			}
			else
			if constexpr (sys::win32)
			{
				return sys::env::get("COMSPEC");
			}
		}

	} SHELL;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			if constexpr (sys::posix)
			{
				return sys::env::get("PS1");
			}
			else
			if constexpr (sys::win32)
			{
				return sys::env::get("PROMPT");
			}
		}

	} PROMPT;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			if constexpr (sys::posix)
			{
				return sys::env::get("DESKTOP_SESSION");
			}
			else
			if constexpr (sys::win32)
			{
				return sys::env::get("OS");
			}
		}

	} DESKTOP;
}

namespace env
{
	list const& path = PATH;
	view const& user = USER;
	view const& home = HOME;
	view const& pwd = PWD;
	view const& lang = LANG;
	view const& shell = SHELL;
	view const& tmpdir = TMPDIR;
	view const& desktop = DESKTOP;
	view const& prompt = PROMPT;
}

