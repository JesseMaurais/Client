// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "err.hpp"
#include "env.hpp"
#include "usr.hpp"
#include "opt.hpp"
#include "arg.hpp"
#include "cmd.hpp"
#include "ini.hpp"
#include "dir.hpp"
#include "fmt.hpp"
#include "type.hpp"
#include "tag.hpp"
#include "sys.hpp"
#include "sync.hpp"
#include <exception>
#include <fstream>
#include <vector>
#include <regex>

namespace env
{
	static sys::rwlock lock;

	bool got(fmt::string::view u)
	{
		if (not fmt::terminated(u))
		{
			auto const s = fmt::to_string(u);
			return env::got(s);
		}
		auto const c = u.data();
		auto const unlock = lock.reader();
		auto const ptr = std::getenv(c);
		return nullptr == ptr;
	}

	fmt::string::view get(fmt::string::view u)
	{
		if (not fmt::terminated(u))
		{
			auto const s = fmt::to_string(u);
			return env::get(s);
		}
		auto const c = u.data();
		auto const unlock = lock.reader();
		auto const ptr = std::getenv(c);
		return nullptr == ptr ? "" : ptr;
	}

	bool set(fmt::string::view u)
	{
		if (not fmt::terminated(u))
		{
			return env::put(u);
		}
		auto const unlock = lock.writer();
		auto const d = u.data();
		auto c = const_cast<char*>(d);
		return 0 != sys::putenv(c);
	}

	bool put(fmt::string::view u)
	{
		static fmt::string::set buf;
		auto const unlock = lock.writer();
		auto it = buf.emplace(u).first;
		auto d = it->data();
		auto c = const_cast<char*>(d);
		return 0 != sys::putenv(c);
	}

	bool put(fmt::string::view u, fmt::string::view v)
	{
		fmt::string::view::vector w { u, v };
		return env::put(fmt::join(w, "="));
	}

	fmt::string::view echo(fmt::string::view u)
	{
		static env::shell sh;
		auto const p = sh.echo(u);
		return p.empty() ? fmt::tag::empty : p[0];
	}
}

namespace env::var
{
	fmt::string::view::span all()
	{
		static thread_local fmt::string::view::vector local;
		local.clear();
		for (char** c = ::sys::environ(); *c; ++c) local.emplace_back(*c);
		return local;
	}

	fmt::string::view::span path()
	{
		static thread_local fmt::string::view::vector t;
		auto u = env::get("PATH");
		t = fmt::path::split(u);
		return t;
	}

	fmt::string::view temp()
	{
		for (auto u : { "TMPDIR", "TEMP", "TMP" })
		{
			if (auto v = env::get(u); not empty(v))
			{
				return v;
			}
		}
		static fmt::string s;
		if (empty(s))
		{
			#ifdef _WIN32
			{
				s = fmt::dir::join({env::var::root(), "Temp"});
			}
			#else
			{
				s = fmt::dir::join({env::var::base(), "tmp"});
			}
			#endif
		}
		return s;
	}

	fmt::string::view pwd()
	{
		#ifdef _WIN32
		{
			static thread_local char buf[FILENAME_MAX];
			return sys::getcwd(buf, sizeof buf);
		}
		#else
		{
			return env::get("PWD");
		}
		#endif
	}

	fmt::string::view home()
	{
		#ifdef _WIN32
		{
			return env::get("USERPROFILE");
		}
		#else
		{
			return env::get("HOME");
		}
		#endif
	}

	fmt::string::view user()
	{
		#ifdef _WIN32
		{
			return env::get("USERHOME");
		}
		#else
		{
			return env::get("USER");
		}
		#endif
	}

	fmt::string::view host()
	{
		#ifdef _WIN32
		{
			return env::get("COMPUTERNAME");
		}
		#else
		{
			static thread_local char buf[64];
			if (sys::fail(gethostname(buf, sizeof buf)))
			{
				sys::err(here, "gethostname");
				std::memset(buf, 0, sizeof buf);
			}
			return buf;
		}
		#endif
	}

	fmt::string::view domain()
	{
		#ifdef _WIN32
		{
			return env::get("USERDOMAIN");
		}
		#else
		{
			static thread_local char buf[64];
			if (sys::fail(getdomainname(buf, sizeof buf)))
			{
				sys::err(here, "getdomainname");
				std::memset(buf, 0, sizeof buf);
			}
			return buf;
		}
		#endif
	}

	fmt::string::view base()
	{
		#ifdef _WIN32
		{
			return env::get("SYSTEMDRIVE");
		}
		#else
		{
			return fmt::tag::empty; // omit "/" for join
		}
		#endif
	}

	fmt::string::view root()
	{
		#ifdef _WIN32
		{
			return env::get("SYSTEMROOT");
		}
		#else
		{
			return "/root";
		}
		#endif
	}

	fmt::string::view lang()
	{
		for (auto u : { "LC_ALL", "LC_MESSAGES", "LANG" })
		{
			if (auto v = env::get(u); not empty(v))
			{
				return v;
			}
		}
		return fmt::tag::empty;
	}

	fmt::string::view shell()
	{
		#ifdef _WIN32
		{
			return env::get("ComSpec");
		}
		#else
		{
			return env::get("SHELL");
		}
		#endif
	}

	fmt::string::view session()
	{
		#ifdef _WIN32
		{
			return env::get("OS");
		}
		#else
		{
			return env::get("DESKTOP_SESSION");
		}
		#endif
	}
}

#ifdef test_unit
test_unit(env)
{
	assert(env::get("PATH") == fmt::path::join(env::var::path()));
	assert(env::get("PATH") == env::echo("PATH"));
}
#endif
