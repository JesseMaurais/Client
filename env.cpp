#include "env.hpp"
#include "fmt.hpp"
#include "sys.hpp"
#include "os.hpp"
#include <map>
#include <regex>
#include <cassert>

namespace sys::env
{
	std::string_view get(std::string_view u)
	{
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
		static std::regex x { "$[A-Z_][A-Z_0-9]*" };
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
			for (auto var : { "LC_ALL", "LANG" })
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
			return sys::env::get("TERM");
		}

	} TERM;

	struct : env::view
	{
		operator std::string_view() const final
		{
			return sys::env::get("PAGER");
		}

	} PAGER;

	struct : env::view
	{
		operator std::string_view() const final
		{
			return sys::env::get("EDITOR");
		}

	} EDITOR;

	struct : env::view
	{
		operator std::string_view() const final
		{
			return sys::env::get("VISUAL");
		}

	} VISUAL;

	struct : env::view
	{
		operator std::string_view() const final
		{
			return sys::env::get("BROWSER");
		}

	} BROWSER;

	struct : env::view
	{
		operator std::string_view() const final
		{
			return sys::env::get("RANDOM");
		}

	} RANDOM;

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

	} DESKTOP_SESSION;
}

namespace env
{
	list const& path = PATH;
	view const& pwd = PWD;
	view const& user = USER;
	view const& home = HOME;
	view const& lang = LANG;
	view const& tmpdir = TMPDIR;
	view const& shell = SHELL;
	view const& prompt = PROMPT;
	view const& term = TERM;
	view const& pager = PAGER;
	view const& editor = EDITOR;
	view const& visual = VISUAL;
	view const& browser = BROWSER;
	view const& random = RANDOM;
	view const& desktop = DESKTOP_SESSION;
}

namespace
{
	struct : env::list
	{
		operator std::vector<std::string_view>() const final
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

	} ENVIRON;
}

env::list const& sys::environment = ENVIRON;

