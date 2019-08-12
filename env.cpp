// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "env.hpp"
#include "fmt.hpp"
#include "dir.hpp"
#include "sys.hpp"
#include "err.hpp"
#include <vector>
#include <regex>

#ifdef _WIN32
# include "win.hpp"
#else
# include "uni.hpp"
#endif

namespace
{
	sys::rwlock lock;
}

namespace sys::env
{
	fmt::string_view get(fmt::string_view u)
	{
		if (not fmt::terminated(u))
		{
			auto const s = fmt::to_string(u);
			return get(s);
		}
		auto const v = data(u);
		auto const unlock = lock.read();
		auto const w = std::getenv(v);
		return nullptr == w ? fmt::nil : w;
	}

	bool set(fmt::string_view u)
	{
		if (not fmt::terminated(u))
		{
			return put(u);
		}
		auto const unlock = lock.write();
		auto const c = const_cast<char*>(data(u));
		return 0 != sys::putenv(c);
	}

	bool put(fmt::string_view u)
	{
		static fmt::string_vector buf;
		auto const unlock = lock.write();
		auto const c = buf.emplace_back(u).data();
		return 0 != sys::putenv(c);
	}

	bool put(fmt::string_view u, fmt::string_view v)
	{
		return put(fmt::entry(u, v));
	}

	static auto val(fmt::string_view u)
	{
		assert('$' == u.front());
		u = u.substr(1);
		if (empty(u))
		{
			return u;
		}
		return get(u);
	}

	fmt::string value(fmt::string_view u)
	{
		static std::regex x { "\\$[A-Z_][A-Z_0-9]*" };
		std::smatch m;
		std::string r;
		auto s = fmt::to_string(u);
		while (std::regex_search(s, m, x))
		{
			r += m.prefix();
			auto t = m.str();
			auto v = val(t);
			r.append(v.data(), v.size());
			s = m.suffix();
		}
		r.shrink_to_fit();
		return r;
	}
}

namespace
{
	struct : env::span
	{
		operator fmt::string_view_span() const final
		{
			static thread_local fmt::string_view_vector t;
			auto u = sys::env::get("PATH");
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
				static thread_local char name[64];
				if (sys::fail(gethostname(name, sizeof name)))
				{
					sys::err(here, "gethostname");
					name[0] = fmt::null;
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
				static thread_local char name[64];
				if (sys::fail(getdomainname(name, sizeof name)))
				{
					sys::err(here, "getdomainname");
					name[0] = fmt::null;
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
				return fmt::nil; // omit "/" for join
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
				static thread_local char buf[FILENAME_MAX];
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
			for (auto u : { "LC_ALL", "LC_MESSAGES", "LANG" })
			{
				auto v = sys::env::get(u);
				if (not empty(v))
				{
					return v;
				}
			}
			return fmt::nil;
		}

	} LANG;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			for (auto u : { "TMPDIR", "TEMP", "TMP" })
			{
				auto v = sys::env::get(u);
				if (not empty(v))
				{
					return v;
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
	span const& paths = PATH;
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

