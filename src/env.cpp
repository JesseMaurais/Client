// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "env.hpp"
#include "fmt.hpp"
#include "dir.hpp"
#include "type.hpp"
#include "sys.hpp"
#include "err.hpp"
#include "sync.hpp"
#include <vector>
#include <regex>

namespace
{
	sys::rwlock lock;
}

namespace env::var
{
	bool got(fmt::string::view u)
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

	fmt::string::view get(fmt::string::view u)
	{
		if (not fmt::terminated(u))
		{
			auto const s = fmt::to_string(u);
			return get(s);
		}
		auto const c = u.data();
		auto const unlock = lock.read();
		auto const ptr = std::getenv(c);
		return nullptr == ptr ? "" : ptr;
	}

	bool set(fmt::string::view u)
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

	bool put(fmt::string::view u)
	{
		static fmt::string::set buf;
		auto const unlock = lock.write();
		auto it = buf.emplace(u).first;
		auto d = it->data();
		auto c = const_cast<char*>(d);
		return 0 != sys::putenv(c);
	}

	bool put(fmt::string::view u, fmt::string::view v)
	{
		return put(fmt::join({u, v}, "="));
	}

	static auto evaluate(fmt::string::view u)
	{
		assert(u.front() == '$');
		u = u.substr(1);
		return empty(u) ? u : get(u);
	}

	fmt::string value(fmt::string::view u)
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
			s = m.suffix().str();
		}
		r.shrink_to_fit();
		return r;
	}
}

namespace
{
	struct : env::span
	{
		operator type() const final
		{
			static thread_local fmt::string::view::vector t;
			auto u = env::var::get("PATH");
			t = fmt::path::split(u);
			return t;
		}

	} PATH;

	struct : env::view
	{
		operator type() const final
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
		operator type() const final
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
		operator type() const final
		{
			#ifdef _WIN32
			{
				return env::var::get("COMPUTERNAME");
			}
			#else
			{
				static thread_local char name[64];
				if (sys::uni::fail(gethostname(name, sizeof name)))
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
		operator type() const final
		{
			#ifdef _WIN32
			{
				return env::var::get("USERDOMAIN");
			}
			#else
			{
				static thread_local char name[64];
				if (sys::uni::fail(getdomainname(name, sizeof name)))
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
		operator type() const final
		{
			#ifdef _WIN32
			{
				return env::var::get("SYSTEMDRIVE");
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
		operator type() const final
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
		operator type() const final
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
		operator type() const final
		{
			for (auto u : { "LC_ALL", "LC_MESSAGES", "LANG" })
			{
				auto v = env::var::get(u);
				if (not empty(v))
				{
					return v;
				}
			}
			return "";
		}

	} LANG;

	struct : env::view
	{
		operator type() const final
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
		operator type() const final
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
		operator type() const final
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
		operator type() const final
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

	} SESSION;
}

namespace env
{
	span::ref paths = PATH;
	view::ref user = USER;
	view::ref home = HOME;
	view::ref host = HOST;
	view::ref domain = DOMAIN_;
	view::ref root = ROOT;
	view::ref pwd = PWD;
	view::ref lang = LANG;
	view::ref shell = SHELL;
	view::ref tmpdir = TMPDIR;
	view::ref rootdir = ROOTDIR;
	view::ref session = SESSION;
	view::ref prompt = PROMPT;
}

#ifdef test
test(env)
{
	assert(env::var::get("PATH") == fmt::path::join(env::paths));
	assert(env::var::get("PATH") == env::var::value("$PATH"));
}
#endif
