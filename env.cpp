// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "env.hpp"
#include "fmt.hpp"
#include "dir.hpp"
#include "sys.hpp"
#include "dbg.hpp"
#include <regex>

namespace sys::env
{
	fmt::string_view get(fmt::string_view u)
	{
		assert(fmt::terminated(u));
		auto const var = u.data();
		auto const val = std::getenv(var);
		return val ? val : "";
	}

	bool put(fmt::string_view u)
	{
		auto buf = fmt::to_string(u);
		auto const p = buf.data();
		return 0 != sys::putenv(p);
	}

	bool set(fmt::string_view u, fmt::string_view v)
	{
		auto const s = fmt::key_value(u, v);
		return put(s);
	}

	static auto val(fmt::string_view u)
	{
		#ifdef _WIN32
		{
			assert('%' == u.front());
			assert('%' == u.back());
			u = u.substr(1, u.size() - 1);
		}
		#else
		{
			assert('$' == u.front());
			u = u.substr(1);
		}
		#endif

		if (empty(u))
		{
			return u;
		}
		return get(u);
	}

	fmt::string value(fmt::string s)
	{
		static std::regex x { sys::esc::env };
		std::smatch m;
		std::string r;
		while (std::regex_search(s, m, x))
		{
			r += m.prefix();
			auto t = m.str();
			auto u = val(t);
			r.append(u.data(), u.size());
			s = m.suffix();
		}
		r.shrink_to_fit();
		return r;
	}
}

namespace
{
	struct : env::list
	{
		operator fmt::string_view_span() const final
		{
			static thread_local fmt::string_view_vector t;
			fmt::string_view u = sys::env::get("PATH");
			t = fmt::path::split(u);
			return t;
		}

	} PATH;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			#ifdef _WIN32
			{
				return sys::env::get("USERNAME");
			}
			#else
			{
				return sys::env::get("USER");
			}
			#endif
		}

	} USER;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			#ifdef _WIN32
			{
				return sys::env::get("USERPROFILE");
			}
			#else
			{
				return sys::env::get("HOME");
			}
			#endif
		}

	} HOME;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			#ifdef _WIN32
			{
				return sys::env::get("COMPUTERNAME");
			}
			#else
			{
				static char name[64];
				if (sys::fail(gethostname(name, sizeof name)))
				{
					sys::err(here, "gethostname");
					name[0] = '\0';
				}
				return name;
			}
			#endif
		}

	} HOST;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			#ifdef _WIN32
			{
				return sys::env::get("USERDOMAIN");
			}
			#else
			{
				static char name[64];
				if (sys::fail(getdomainname(name, sizeof name)))
				{
					sys::err(here, "getdomainname");
					name[0] = '\0';
				}
				return name;
			}
			#endif
		}

	} DOMAIN_;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			#ifdef _WIN32
			{
				return sys::env::get("SYSTEMDRIVE");
			}
			#else
			{
				return ""; // omit "/" for join
			}
			#endif
		}

	} ROOT;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			#ifdef _WIN32
			{
				return sys::env::get("SYSTEMROOT");
			}
			#else
			{
				return "/root";
			}
			#endif
		}

	} ROOTDIR;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			#ifdef _WIN32
			{
				static char buf[FILENAME_MAX];
				return sys::getcwd(buf, sizeof buf);
			}
			#else
			{
				return sys::env::get("PWD");
			}
			#endif
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
				auto u = sys::env::get(var);
				if (not empty(u))
				{
					return u;
				}
			}
			static fmt::string s;
			if (empty(s))
			{
				#ifdef _WIN32
				{
					s = fmt::dir::join(env::rootdir, "Temp");
				}
				#else
				{
					s = fmt::dir::join(env::root, "tmp");
				}
				#endif
			}
			return s;
		}

	} TMPDIR;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			#ifdef _WIN32
			{
				return sys::env::get("COMSPEC");
			}
			#else
			{
				return sys::env::get("SHELL");
			}
			#endif
		}

	} SHELL;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			#ifdef _WIN32
			{
				return sys::env::get("PROMPT");
			}
			#else
			{
				return sys::env::get("PS1");
			}
			#endif
		}

	} PROMPT;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			#ifdef _WIN32
			{
				return sys::env::get("OS");
			}
			#else
			{
				return sys::env::get("DESKTOP_SESSION");
			}
			#endif
		}

	} DESKTOP;
}

namespace env
{
	list const& paths = PATH;
	view const& user = USER;
	view const& home = HOME;
	view const& host = HOST;
	view const& domain = DOMAIN_;
	view const& root = ROOT;
	view const& pwd = PWD;
	view const& lang = LANG;
	view const& shell = SHELL;
	view const& tmpdir = TMPDIR;
	view const& rootdir = ROOTDIR;
	view const& desktop = DESKTOP;
	view const& prompt = PROMPT;
}

