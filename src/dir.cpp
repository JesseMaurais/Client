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
	fmt::string impl::join(fmt::string_view_span p)
	{
		return fmt::lc.join(p, sys::sep::dir);
	}

	fmt::string_view_vector split(fmt::string_view u)
	{
		return fmt::split(u, sys::sep::dir);
	}
}

namespace fmt::path
{
	fmt::string impl::join(fmt::string_view_span p)
	{
		return fmt::lc.join(p, sys::sep::path);
	}

	fmt::string_view_vector split(fmt::string_view u)
	{
		return fmt::split(u, sys::sep::path);
	}
}

namespace sys::path
{
	bool fail(fmt::string_view path, file::mode um)
	{
		if (not fmt::terminated(path))
		{
			auto const s = fmt::to_string(path);
			return sys::path::fail(s);
		}

		auto const c = path.data();
		return sys::file::fail(c, um);
	}
}

namespace sys::dir
{
	bool fail(fmt::string_view path, file::mode am)
	{
		if (not fmt::terminated(path))
		{
			auto const s = fmt::to_string(path);
			return sys::dir::fail(s);
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
}

namespace env::dir
{
	bool find(fmt::string_view path, entry view)
	{
		if (not fmt::terminated(path))
		{
			auto const s = fmt::to_string(path);
			return env::dir::find(s, view);
		}

		auto const c = data(path);
		for (auto const name : sys::files(c))
		{
			if (view(name)) return success;
		}
		return failure;
	}

	fmt::string_view make(fmt::string_view path)
	{
		std::stack<fmt::string_view> stack;
		fmt::string buf;

		auto folders = fmt::dir::split(path);
		auto stem = path;

		while (sys::dir::fail(stem))
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
			auto const c = data(buf);
			if (sys::fail(sys::mkdir(c, S_IRWXU)))
			{
				sys::err(here, "mkdir", c);
				stem = "";
				break;
			}
		}

		return stem;
	}

	bool remove(fmt::string_view dir)
	{
		std::deque<fmt::string> deque;
		deque.emplace_back(dir);

		for (auto it = deque.begin(); it != deque.end(); ++it)
		{
			auto const d = *it;
			(void) find(d, [&](fmt::string_view u)
			{
				auto const path = fmt::dir::join(d, u);
				auto const c = data(path);
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

	entry mode(sys::file::mode bit)
	{
		auto const flags = sys::file::convert(bit);
		return [=](fmt::string_view u)
		{
			auto const s = u.data();
			if (sys::fail(sys::access(s, flags)))
			{
				return false;
			}
			#ifdef _WIN32
			{
				if (bit & sys::file::ex)
				{
					DWORD type;
					BOOL ok = GetBinaryType(s, &type);
					return TRUE == ok;
				}
			}
			#endif
			return true;
		};
	}

	entry match(fmt::string_view u)
	{
		auto const buf = fmt::to_string(u);
		auto const x = std::regex(buf);
		return [x](fmt::string_view u)
		{
			std::cmatch m;
			auto const s = u.data();
			return std::regex_search(s, m, x);
		};
	}

	entry insert(fmt::string_vector& buf)
	{
		return [&](fmt::string_view u)
		{
			auto s = fmt::to_string(u);
			buf.emplace_back(move(s));
			return false;
		};
	}

	entry copy(fmt::string& buf)
	{
		return [&](fmt::string_view u)
		{
			buf = fmt::to_string(u);
			return false;
		};
	}
}

