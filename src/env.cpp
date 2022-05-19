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

	bool got(fmt::view u)
	{
		if (not fmt::terminated(u))
		{
			const auto s = fmt::to_string(u);
			return env::got(s);
		}
		const auto c = u.data();
		const auto unlock = lock.reader();
		const auto ptr = std::getenv(c);
		return nullptr == ptr;
	}

	fmt::view get(fmt::view u)
	{
		if (not fmt::terminated(u))
		{
			const auto s = fmt::to_string(u);
			return env::get(s);
		}
		const auto c = u.data();
		const auto unlock = lock.reader();
		const auto ptr = std::getenv(c);
		return nullptr == ptr ? "" : ptr;
	}

	bool set(fmt::view u)
	{
		if (not fmt::terminated(u))
		{
			return env::put(u);
		}
		const auto unlock = lock.writer();
		const auto d = u.data();
		const auto c = fwd::non_const(d);
		return 0 != sys::putenv(c);
	}

	bool put(fmt::view u)
	{
		static fmt::string::set buf;
		const auto unlock = lock.writer();
		const auto it = buf.emplace(u).first;
		const auto d = it->data();
		const auto c = fwd::non_const(d);
		return 0 != sys::putenv(c);
	}

	bool put(fmt::view u, fmt::view v)
	{
		fmt::vector w { u, v };
		const auto s = fmt::join(w, fmt::tag::assign);
		return env::put(s);
	}

	fmt::view echo(fmt::view u)
	{
		static env::shell sh;
		auto const p = sh.echo(u);
		return p.empty() ? fmt::tag::empty : p[0];
	}
}

namespace env::var
{
	fmt::span all()
	{
		static thread_local fmt::vector local;
		local.clear();
		for (auto c = sys::environ(); *c; ++c) local.emplace_back(*c);
		return local;
	}

	fmt::span path()
	{
		static thread_local fmt::vector t;
		auto u = env::get("PATH");
		t = fmt::path::split(u);
		return t;
	}

	fmt::view temp()
	{
		for (auto u : { "TMPDIR", "TEMP", "TMP" })
		{
			if (auto v = env::get(u); not v.empty())
			{
				return v;
			}
		}
		static fmt::string s;
		if (s.empty())
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

	fmt::view pwd()
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

	fmt::view home()
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

	fmt::view user()
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

	fmt::view host()
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

	fmt::view domain()
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

	fmt::view base()
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

	fmt::view root()
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

	fmt::view lang()
	{
		for (auto u : { "LC_ALL", "LC_MESSAGES", "LANG" })
		{
			if (auto v = env::get(u); not v.empty())
			{
				return v;
			}
		}
		return fmt::tag::empty;
	}

	fmt::view shell()
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

	fmt::view session()
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
