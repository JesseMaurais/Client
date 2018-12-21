#include "env.hpp"
#include "fmt.hpp"
#include "sys.hpp"
#include "os.hpp"

namespace sys::env
{
	std::string_view get(std::string_view s)
	{
		return std::getenv(s.data());
	}

	bool set(std::string_view k, std::string_view v)
	{
		std::string s = fmt::key_value(k, v);
		return sys::putenv(s.data()) != 0;
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

	struct TMPDIR : env::view
	{
		operator std::string_view() const final
		{
			for (auto var : { "TMPDIR", "TEMP", "TMP" })
			{
				auto view = sys::env::get(var);
				if (not view.empty())
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

	struct RANDOM : env::view
	{
		operator std::string_view() const final
		{
			return sys::env::get("RANDOM");
		}
	}
}

namespace env
{
	list const& path   = PATH();
	view const& pwd    = PWD();
	view const& user   = USER();
	view const& home   = HOME();
	view const& tmpdir = TMPDIR();
	view const& shell  = SHELL();
	view const& term   = TERM();
	view const& pager  = PAGER();
	view const& editor = EDITOR();
	view const& visual = VISUAL();
	view const& random = RANDOM();
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

