#include "env.hpp"
#include "fmt.hpp"
#include "sys.hpp"
#include "os.hpp"
#include <map>
#include <regex>

namespace sys::env
{
	std::string_view get(std::string_view u)
	{
		auto const p = u.data();
		return std::getenv(p);
	}

	bool put(std::string_view u)
	{
		auto const p = u.data();
		return 0 != sys::putenv(p);
	}

	bool set(std::string_view u, std::string_view v)
	{
		std::string const s = fmt::key_value(u, v);
		return putenv(s);
	}

	std::string format(std::string u)
	{
		static std::regex x { "$[A-Z_][A-Z_0-9]*" };
		std::smatch m;
		std::string s;
		while (std::regex_search(u, m, x))
		{
			s += m.prefix();
			auto t = m.str();
			s += get(t);
			u = m.suffix();
		}
		s.shrink_to_fit();
		return s;
	}
}

namespace
{
	struct PATH : env::list
	{
		operator std::vector<std::string_view>() const final
		{
			std::string_view view = sys::env::get("PATH");
			return fmt::split(view, sys::sep::path);
		}
	}

	struct PWD : env::view
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
	}

	struct USER : env::view
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
	};

	struct HOME : env::view
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
	};

	struct LANG : env::view
	{
		operator std::string_view() const final
		{
			for (auto var : { "LC_ALL", "LANG" })
			{
				auto view = sys::env::get(var);
				if (empty(view))
				{
					return view;
				}
			}
			return "";
		}
	};

	struct TMPDIR : env::view
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
	};

	struct SHELL : env::view
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
	};

	struct PROMPT : env::view
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
	};

	struct TERM : env::view
	{
		operator std::string_view() const final
		{
			return sys::env::get("TERM");
		}
	};

	struct PAGER : env::view
	{
		operator std::string_view() const final
		{
			return sys::env::get("PAGER");
		}
	};

	struct EDITOR : env::view
	{
		operator std::string_view() const final
		{
			return sys::env::get("EDITOR");
		}
	};

	struct VISUAL : env::view
	{
		operator std::string_view() const final
		{
			return sys::env::get("VISUAL");
		}
	};

	struct BROWSER : env::view
	{
		operator std::string_view() const final
		{
			return sys::env::get("BROWSER");
		}
	};

	struct RANDOM : env::view
	{
		operator std::string_view() const final
		{
			return sys::env::get("RANDOM");
		}
	};

	struct DESKTOP : env::view
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
	};
}

namespace env
{
	list const& path = PATH();
	view const& pwd = PWD();
	view const& user = USER();
	view const& home = HOME();
	view const& lang = LANG();
	view const& tmpdir = TMPDIR();
	view const& shell = SHELL();
	view const& prompt = PROMPT();
	view const& term = TERM();
	view const& pager = PAGER();
	view const& editor = EDITOR();
	view const& visual = VISUAL();
	view const& browser = BROWSER();
	view const& random = RANDOM();
	view const& desktop = DESKTOP();
}

namespace
{
	struct ENVIRON : env::list
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
	};
}

env::list const& sys::environment = ENVIRON();

