#include "env.hpp"
#include "fmt.hpp"
#include "sys.hpp"
#include "os.hpp"
#include <regex>
#include <cassert>

namespace sys::env
{
	std::string_view get(std::string_view u)
	{
		assert(fmt::terminated(u));
		auto const p = u.data();
		return std::getenv(p);
	}

	bool put(std::string_view u)
	{
		assert(fmt::terminated(u));
		auto p = const_cast<char*>(u.data());
		return 0 != sys::putenv(p);
	}

	bool set(std::string_view u, std::string_view v)
	{
		auto const s = fmt::key_value(u, v);
		return put(s);
	}

	std::string eval(std::string u)
	{
		static std::regex x { sys::sh::esc::regex };
		std::smatch m;
		std::string s;
		while (std::regex_search(u, m, x))
		{
			s += m.prefix();
			auto t = m.str();
			auto v = get(t);
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
		operator std::vector<std::string_view>() const final
		{
			std::string_view u = sys::env::get("PATH");
			return fmt::split(u, sys::sep::path);
		}

	} PATH;

	struct : env::view
	{
		operator std::string_view() const final
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

	} USER;

	struct : env::view
	{
		operator std::string_view() const final
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

	} HOME;

	struct : env::view
	{
		operator std::string_view() const final
		{
			if constexpr (sys::POSIX)
			{
				return sys::env::get("PWD");
			}
			else
			if constexpr (sys::WIN32)
			{
				return sys::env::get("CD");
			}
		}

	} PWD;

	struct : env::view
	{
		operator std::string_view() const final
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
		operator std::string_view() const final
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
		operator std::string_view() const final
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

	} SHELL;

	struct : env::view
	{
		operator std::string_view() const final
		{
			if constexpr (sys::POSIX)
			{
				return sys::env::get("PS1");
			}
			else
			if constexpr (sys::WIN32)
			{
				return sys::env::get("PROMPT");
			}
		}

	} PROMPT;

	struct : env::view
	{
		operator std::string_view() const final
		{
			if constexpr (sys::POSIX)
			{
				return sys::env::get("DESKTOP_SESSION");
			}
			else
			if constexpr (sys::WIN32)
			{
				return sys::env::get("OS");
			}
		}

	} DESKTOP;
}

namespace env
{
	list const& PATH = path;
	view const& USER = user;
	view const& HOME = home;
	view const& PWD = pwd;
	view const& LANG = lang;
	view const& TMPDIR = tmpdir;
	view const& SHELL = shell;
	view const& PROMPT = prompt;
	view const& DESKTOP = desktop;
}

