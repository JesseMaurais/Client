// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "err.hpp"
#include "env.hpp"
#include "exe.hpp"
#include "arg.hpp"
#include "dir.hpp"
#include "fmt.hpp"
#include "sys.hpp"
#include "dig.hpp"
#include "type.hpp"
#include "time.hpp"
#include "sync.hpp"

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

	fmt::span vars()
	{
		static thread_local fmt::vector local;
		local.clear();
		for (auto c = sys::environment(); *c; ++c)
		{
			local.emplace_back(*c);
		}
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
				s = fmt::dir::join({env::root(), "Temp"});
			}
			#else
			{
				s = fmt::dir::join({env::base(), "tmp"});
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
			return env::get("UserHome");
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
			return env::get("UserProfile");
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
			return env::get("ComputerName");
		}
		#else
		{
			static thread_local char buf[64];
			if (sys::fail(gethostname(buf, sizeof buf)))
			{
				perror("gethostname");
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
			return env::get("UserDomain");
		}
		#else
		{
			static thread_local char buf[64];
			if (sys::fail(getdomainname(buf, sizeof buf)))
			{
				perror("getdomainname");
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
			return env::get("SystemDrive");
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
			return env::get("SystemRoot");
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

	fmt::view echo(fmt::view u)
	{
		const auto p = env::exe::echo(u);
		return p.empty() ? "" : p.front();
	}

	fmt::view text(fmt::view u)
	{
		static auto cat = fmt::catalog(env::opt::application());
		return cat(u);
	}
/*
	fmt::output print(fmt::output out, fmt::view format, fmt::init params)
	{
		return print(out, format, fwd::to_span(params));
	}

	fmt::output print(fmt::output out, fmt::view format, fmt::span params)
	{
		constexpr auto token = '%';
		auto suffix = text(format);

		while (not suffix.empty())
		{
			auto it = fwd::find(suffix, token);
			auto before = suffix.before(it);
			auto after = suffix.after(it);
			auto end = fmt::skip(after, fmt::alnum);
			suffix = suffix.sub(end);

			out << before;

			if (auto next = after.before(end); next.empty())
			{
				if (after.end() == end or *end == token)
				{
					out << token;
					++ end;
				}
			}
			else
			if (fmt::all_of(next, fmt::digit))
			{
				const auto index = fmt::to_long(next);
				out << params[index];
			}
			else
			if (auto value = env::opt::get(next); not value.empty())
			{
				out << value;
			}
		}
		return out;
	}
*/
}

#ifdef test_unit

test_unit(path)
{
	assert(env::get("PATH") == fmt::path::join(env::path()));
	assert(env::get("PATH") == env::echo("PATH"));
}
/*
test_unit(print)
{
	std::stringstream ss;

	env::print(ss, "The text \"%~1, %0!\" is an example.", {"World", "Hello"});
	assert(ss.str() == "The text \"Hello, World!\" is an example.");
	ss.str("");

	env::print(ss, "The variable %UNKNOWN should be filtered.", {});
	assert(ss.str() == "The variable should be filtered.");
	ss.str("");
}
*/
#endif
