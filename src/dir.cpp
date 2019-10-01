// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dir.hpp"
#include "err.hpp"
#include "fmt.hpp"
#include "usr.hpp"
#include "opt.hpp"
#include "sys.hpp"
#include "direct.hpp"
#include <algorithm>
#include <regex>
#include <stack>

namespace fmt::dir
{
	string join(span_view p)
	{
		return fmt::lc.join(p, sys::sep::dir);
	}

	list_view split(view u)
	{
		return fmt::split(u, sys::sep::dir);
	}
}

namespace fmt::path
{
	string join(span_view p)
	{
		return fmt::lc.join(p, sys::sep::path);
	}

	list_view split(view u)
	{
		return fmt::split(u, sys::sep::path);
	}
}

namespace env::dir
{
	using namespace ::env;
	using namespace ::env::usr;

	bool path(entry look)
	{
		return find(pwd, look) or find(paths, look);
	}

	bool config(entry look)
	{
		return find(config_home, look) or find(config_dirs, look);
	}

	bool data(entry look)
	{
		return find(data_home, look) or find(data_dirs, look);
	}

	bool cache(entry look)
	{
		return find(cache_home, look);
	}

	bool find(fmt::view path, entry look)
	{
		if (not fmt::terminated(path))
		{
			auto const s = fmt::to_string(path);
			return env::dir::find(s, look);
		}

		auto const c = path.data();
		for (auto const name : sys::files(c))
		{
			if (look(name)) 
			{
				return success;
			}
		}
		return failure;
	}

	bool find(fmt::span_view list, entry look)
	{
		for (auto const path : list)
		{
			if (find(path, look))
			{
				return true;
			}
		}
		return false;
	}

	entry mask(file::mode am)
	{
		auto const flags = file::convert(am);
		return [=](fmt::view u)
		{
			auto const c = u.data();
			if (file::fail(sys::access(c, flags)))
			{
				return failure;
			}
			#ifdef _WIN32
			{
				if (am & file::ex)
				{
					DWORD dw;
					return GetBinaryType(c, &dw)
						? success : failure;
				}
			}
			#endif
			return success;
		};
	}

	entry regx(fmt::view u)
	{
		auto const s = fmt::to_string(u);
		auto const x = std::regex(s);
		return [x](fmt::view u)
		{
			std::cmatch cm;
			auto const s = fmt::to_string(u);
			return std::regex_search(s.data(), cm, x);
		};
	}

	entry to(fmt::alloc& t)
	{
		return [&](fmt::view u)
		{
			auto s = fmt::to_string(u);
			t.emplace_back(move(s));
			return success;
		};
	}

	entry to(fmt::string &s)
	{
		return [&](fmt::view u)
		{
			s = fmt::to_string(u);
			return success;
		};
	}

	bool fail(fmt::view path, file::mode am)
	{
		if (not fmt::terminated(path))
		{
			auto const s = fmt::to_string(path);
			return env::dir::fail(s);
		}

		auto const c = path.data();
		class sys::stat st(c);
		if (file::fail(st))
		{
			return failure;
		}

		if (not S_ISDIR(st.st_mode))
		{
			return failure;
		}

		auto const mask = file::check(am);
		return st.st_mode & mask;
	}

	fmt::view make(fmt::view path)
	{
		std::stack<fmt::view> stack;
		fmt::string buf;

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

	bool remove(fmt::view dir)
	{
		std::deque<fmt::string> deque;
		deque.emplace_back(dir);

		for (auto it = deque.begin(); it != deque.end(); ++it)
		{
			(void) find(*it, [&](fmt::view u)
			{
				auto const path = fmt::dir::join({*it, u});
				auto const c = path.data();
				class sys::stat st(c);
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
