// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dir.hpp"
#include "err.hpp"
#include "fmt.hpp"
#include "usr.hpp"
#include "opt.hpp"
#include "sys.hpp"
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
	string join(span<view> p)
	{
		return fmt::lc.join(p, sys::sep::dir);
	}

	vector split(view u)
	{
		return fmt::split(u, sys::sep::dir);
	}
}

namespace fmt::path
{
	string join(span<view> p)
	{
		return fmt::lc.join(p, sys::sep::path);
	}

	vector split(view u)
	{
		return fmt::split(u, sys::sep::path);
	}
}

namespace env::sys
{
	using namespace ::sys;
}

namespace env::dir
{
	bool find(fmt::view path, entry peek)
	{
		if (not fmt::terminated(path))
		{
			auto const s = fmt::to_string(path);
			return env::dir::find(s, peek);
		}

		auto const c = path.data();
		for (auto const name : sys::files(c))
		{
			if (peek(name)) return success;
		}
		return failure;
	}

	bool fail(fmt::view path, mode am)
	{
		if (not fmt::terminated(path))
		{
			auto const s = fmt::to_string(path);
			return env::dir::fail(s);
		}

		auto const c = path.data();
		class sys::stat st(c);
		if (sys::fail(st))
		{
			return failure;
		}

		if (not S_ISDIR(st.st_mode))
		{
			return failure;
		}

		auto const mask = sys::file::check(am);
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
			if (sys::fail(sys::mkdir(c, S_IRWXU)))
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
				auto const c = path.c_str();
				class sys::stat st(c);
				if (sys::fail(st))
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
				if (sys::fail(sys::unlink(c)))
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
			auto const c = data(dir);
			if (sys::fail(sys::rmdir(c)))
			{
				sys::err(here, "rmdir", c);
				ok = failure;
			}
			deque.pop_back();
		}
		return ok;
	}

	entry mask(mode am)
	{
		auto const flags = sys::file::convert(am);
		return [=](fmt::view u)
		{
			auto const s = u.data();
			if (sys::fail(sys::access(s, flags)))
			{
				return failure;
			}
			#ifdef _WIN32
			{
				if (am & sys::file::ex)
				{
					DWORD type;
					BOOL ok = GetBinaryType(s, &type);
					return TRUE == ok ? success : failure;
				}
			}
			#endif
			return success;
		};
	}

	entry regx(fmt::view u)
	{
		auto const buf = fmt::to_string(u);
		auto const x = std::regex(buf);
		return [x](fmt::view u)
		{
			std::cmatch m;
			auto const s = u.data();
			auto const b = std::regex_search(s, m, x);
			return success == b;
		};
	}

	entry push(fmt::buffer& buf)
	{
		return [&](fmt::view u)
		{
			auto s = fmt::to_string(u);
			buf.emplace_back(move(s));
			return success;
		};
	}

	entry copy(fmt::string& buf)
	{
		return [&](fmt::view u)
		{
			buf = fmt::to_string(u);
			return success;
		};
	}
}
