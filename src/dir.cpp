// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dir.hpp"
#include "err.hpp"
#include "type.hpp"
#include "usr.hpp"
#include "opt.hpp"
#include "sys.hpp"
#include "file.hpp"
#ifdef _WIN32
# include "win/file.hpp"
#else
# include "uni/dirent.hpp"
#endif
#include <algorithm>
#include <regex>
#include <stack>

namespace fmt::dir
{
	string join(string::view::span p)
	{
		return fmt::join(p, sys::sep::dir);
	}

	string join(string::view::init p)
	{
		return fmt::join(p, sys::sep::dir);
	}

	string::view::vector split(string::view u)
	{
		return fmt::split(u, sys::sep::dir);
	}
}

namespace fmt::path
{
	string join(string::view::span p)
	{
		return fmt::join(p, sys::sep::path);
	}

	string join(string::view::init p)
	{
		return fmt::join(p, sys::sep::path);
	}

	string::view::vector split(string::view u)
	{
		return fmt::split(u, sys::sep::path);
	}
}

namespace env::dir
{
	edges paths()
	{
		return { env::pwd(), env::paths() };
	}

	edges config()
	{
		return { env::usr::config_home(), env::usr::config_dirs() };
	}

	edges data()
	{
		return { env::usr::data_home(), env::usr::data_dirs() };
	}

	bool find(view path, entry check)
	{
		if (not fmt::terminated(path))
		{
			auto const s = fmt::to_string(path);
			return env::dir::find(s, check);
		}
		auto const c = path.data();

		return fwd::any_of(sys::files(c), check);
	}

	bool find(span paths, entry check)
	{
		return fwd::any_of(paths, [check](auto path)
		{
			return find(path, check);
		});
	}

	bool find(edges paths, entry look)
	{
		return find(paths.first, look) or find(paths.second, look);
	}

	entry mask(mode am)
	{
		auto const flags = convert(am);
		entry m = [=, &m](view u)->bool
		{
			if (not fmt::terminated(u))
			{
				return m(fmt::to_string(u));
			}
			auto const c = u.data();

			if (sys::fail(sys::access(c, flags)))
			{
				return failure;
			}

			#ifdef _WIN32
			if (am & file::ex)
			{
				DWORD dw;
				return GetBinaryType(c, &dw)
					? success : failure;
			}
			#endif

			return success;
		};
		return m;
	}

	entry regx(view u)
	{
		auto const s = fmt::to_string(u);
		auto const x = std::regex(s);
		return [x](view u)
		{
			std::cmatch cm;
			auto const s = fmt::to_string(u);
			return std::regex_search(s.data(), cm, x);
		};
	}

	entry to(vector& t)
	{
		return [&](view u)
		{
			auto s = fmt::to_string(u);
			t.emplace_back(move(s));
			return success;
		};
	}

	entry to(string& s)
	{
		return [&](view u)
		{
			s = fmt::to_string(u);
			return success;
		};
	}

	entry all(view u, mode m, entry e)
	{
		return mask(m) || regx(u) || e;
	}

	entry any(view u, mode m, entry e)
	{
		return all(u, m, e);
	}

	bool fail(view path, mode am)
	{
		if (not fmt::terminated(path))
		{
			auto const s = fmt::to_string(path);
			return env::dir::fail(s, am);
		}
		auto const c = path.data();
		
		struct sys::stat st(c);
		if (file::fail(st))
		{
			return failure;
		}

		if (not S_ISDIR(st.st_mode))
		{
			return failure;
		}

		auto const mask = check(am);
		return st.st_mode & mask;
	}

	view make(view path)
	{
		std::stack<view> stack;
		string buf;

		auto folders = fmt::dir::split(path);
		auto stem = path;

		while (env::dir::fail(stem))
		{
			stack.push(folders.back());
			folders.pop_back();

			buf = fmt::dir::join(folders);
			stem = buf;
		}

		stem = path.substr(0, path.find(sys::sep::dir, stem.size() + 1));

		while (not empty(stack))
		{
			folders.push_back(stack.top());
			stack.pop();

			buf = fmt::dir::join(folders);
			auto const c = buf.data();
			if (file::fail(sys::mkdir(c, S_IRWXU)))
			{
				sys::err(here, "mkdir", c);
				stem = "";
				break;
			}
		}

		return stem;
	}

	bool remove(view dir)
	{
		std::deque<string> deque;
		deque.emplace_back(dir);

		for (auto it = deque.begin(); it != deque.end(); ++it)
		{
			(void) find(*it, [&](view u)
			{
				auto const path = fmt::dir::join({*it, u});
				auto const c = path.data();
				struct sys::stat st(c);
				if (file::fail(st))
				{
					sys::err(here, "stat", c);
				}
				else
				if (S_ISDIR(st.st_mode))
				{
					if (u != "." and u != "..")
					{
						deque.emplace_back(move(path));
					}
				}
				else
				if (file::fail(sys::unlink(c)))
				{
					sys::err(here, "unlink", c);
				}
				return success;
			});
		}

		bool ok = success;
		while (not empty(deque))
		{
			dir = deque.back();
			auto const c = dir.data();
			if (file::fail(sys::rmdir(c)))
			{
				sys::err(here, "rmdir", c);
				ok = failure;
			}
			deque.pop_back();
		}
		return ok;
	}
}

#ifdef test_unit
test_unit(dir)
{
	assert(not env::dir::fail(env::temp()));
	assert(not env::dir::fail(env::pwd()));
	assert(not env::dir::fail(env::home()));

	auto const path = fmt::dir::split(__FILE__);
	assert(not empty(path));
	auto const name = path.back();
	assert(not empty(name));

	assert(env::dir::find(env::pwd(), [&](auto entry)
	{
		return fmt::dir::split(entry).back() == name;
	}));

	auto const temp = fmt::dir::join({env::temp(), "my", "test", "dir"});
	if (std::empty(temp)) return;
	auto const stem = env::dir::make(temp);
//	assert(not empty(stem.first));
//	assert(not empty(stem.second));
	assert(not env::dir::remove(stem));
}
#endif
