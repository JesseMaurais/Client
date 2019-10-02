// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "env.hpp"
#include "fmt.hpp"
#include "dir.hpp"
#include "sys.hpp"
#include "err.hpp"
#include "thread.hpp"
#include <vector>
#include <regex>

namespace
{
	sys::rwlock lock;
}

namespace env::var
{
	bool got(fmt::view u)
	{
		if (not fmt::terminated(u))
		{
			auto const s = fmt::to_string(u);
			return got(s);
		}
		auto const c = u.data();
		auto const unlock = lock.read();
		auto const ptr = std::getenv(c);
		return nullptr == ptr;
	}

	fmt::view get(fmt::view u)
	{
		if (not fmt::terminated(u))
		{
			auto const s = fmt::to_string(u);
			return get(s);
		}
		auto const c = u.data();
		auto const unlock = lock.read();
		auto const ptr = std::getenv(c);
		return nullptr == ptr ? fmt::nil : ptr;
	}

	bool set(fmt::view u)
	{
		if (not fmt::terminated(u))
		{
			return put(u);
		}
		auto const unlock = lock.write();
		auto const d = u.data();
		auto c = const_cast<char*>(d);
		return 0 != sys::putenv(c);
	}

	bool put(fmt::view u)
	{
		static std::set<fmt::string> buf;
		auto const unlock = lock.write();
		auto it = buf.emplace(u).first;
		auto d = it->data();
		auto c = const_cast<char*>(d);
		return 0 != sys::putenv(c);
	}

	bool put(fmt::view u, fmt::view v)
	{
		return put(fmt::entry(u, v));
	}

	static auto evaluate(fmt::view u)
	{
		assert(u.front() == '$');
		u = u.substr(1);
		return empty(u) ? u : get(u);
	}

	fmt::string value(fmt::view u)
	{
		static std::regex x { "\\$[A-Z_][A-Z_0-9]*" };
		std::smatch m;
		std::string r;
		auto s = fmt::to_string(u);
		while (std::regex_search(s, m, x))
		{
			r += m.prefix();
			auto t = m.str();
			auto v = evaluate(t);
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
		operator fmt::span<fmt::view>() const final
		{
			static thread_local fmt::list_view t;
			auto u = env::var::get("PATH");
			t = fmt::path::split(u);
			return t;
		}

	} PATH;

	struct : env::view
	{
		operator fmt::view() const final
		{
			#ifdef _WIN32
			{
				return env::var::get("USERNAME");
			}
			#else
			{
				return env::var::get("USER");
			}
			#endif
		}

	} USER;

	struct : env::view
	{
		operator fmt::view() const final
		{
			#ifdef _WIN32
			{
				return env::var::get("USERPROFILE");
			}
			#else
			{
				return env::var::get("HOME");
			}
			#endif
		}

	} HOME;

	struct : env::view
	{
		operator fmt::view() const final
		{
			#ifdef _WIN32
			{
				return env::var::get("COMPUTERNAME");
			}
			#else
			{
				static thread_local char name[64];
				if (fail(gethostname(name, sizeof name)))
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
		operator fmt::view() const final
		{
			#ifdef _WIN32
			{
				return env::var::get("USERDOMAIN");
			}
			#else
			{
				static thread_local char name[64];
				if (fail(getdomainname(name, sizeof name)))
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
		operator fmt::view() const final
		{
			#ifdef _WIN32
			{
				return env::var::get("SYSTEMDRIVE");
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
		operator fmt::view() const final
		{
			#ifdef _WIN32
			{
				return env::var::get("SYSTEMROOT");
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
		operator fmt::view() const final
		{
			#ifdef _WIN32
			{
				static thread_local char buf[FILENAME_MAX];
				return sys::getcwd(buf, sizeof buf);
			}
			#else
			{
				return env::var::get("PWD");
			}
			#endif
		}

	} PWD;

	struct : env::view
	{
		operator fmt::view() const final
		{
			for (auto u : { "LC_ALL", "LC_MESSAGES", "LANG" })
			{
				auto v = env::var::get(u);
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
		operator fmt::view() const final
		{
			for (auto u : { "TMPDIR", "TEMP", "TMP" })
			{
				auto v = env::var::get(u);
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
					s = fmt::dir::join({env::rootdir, "Temp"});
				}
				#else
				{
					s = fmt::dir::join({env::root, "tmp"});
				}
				#endif
			}
			return s;
		}

	} TMPDIR;

	struct : env::view
	{
		operator fmt::view() const final
		{
			#ifdef _WIN32
			{
				return env::var::get("COMSPEC");
			}
			#else
			{
				return env::var::get("SHELL");
			}
			#endif
		}

	} SHELL;

	struct : env::view
	{
		operator fmt::view() const final
		{
			#ifdef _WIN32
			{
				return env::var::get("PROMPT");
			}
			#else
			{
				return env::var::get("PS1");
			}
			#endif
		}

	} PROMPT;

	struct : env::view
	{
		operator fmt::view() const final
		{
			#ifdef _WIN32
			{
				return env::var::get("OS");
			}
			#else
			{
				return env::var::get("DESKTOP_SESSION");
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
