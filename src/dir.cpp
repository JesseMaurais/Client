// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dir.hpp"
#include "err.hpp"
#include "fmt.hpp"
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

namespace
{
	struct : env::pair
	{
		operator type() const final
		{
			return { env::pwd, env::paths };
		}

	} PATHS;

	struct : env::pair
	{
		operator type() const final
		{
			return { env::usr::config_home, env::usr::config_dirs };
		}

	} CONFIG;

	struct : env::pair
	{
		operator type() const final
		{
			return { env::usr::data_home, env::usr::data_dirs };
		}

	} DATA;
}

namespace env::dir
{
	order::ref paths = PATHS;
	order::ref config = CONFIG;
	order::ref data = DATA;

	bool find(string::view path, entry look)
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

	bool find(string::view::span paths, entry look)
	{
		for (auto const path : paths)
		{
			if (find(path, look))
			{
				return true;
			}
		}
		return false;
	}

	bool find(string::span paths, entry look)
	{
		for (auto const path : paths)
		{
			if (find(path, look))
			{
				return true;
			}
		}
		return false;
	}

	bool find(string::view::span paths, entry look)
	{
		return find(paths.first, look) or find(paths.second, look);
	}

	entry all(string::view u, file::mode m, entry e)
	{
		return mask(m) || regex(u) || e;
	}

	entry any(string::view u, file::mode m, entry e)
	{
		return all(u, m, e);
	}

	entry mask(file::mode am)
	{
		auto const flags = file::convert(am);
		return [=](fmt::string::view u)
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

	entry regx(fmt::string::view u)
	{
		auto const s = fmt::to_string(u);
		auto const x = std::regex(s);
		return [x](fmt::string::view u)
		{
			std::cmatch cm;
			auto const s = fmt::to_string(u);
			return std::regex_search(s.data(), cm, x);
		};
	}

	entry to(fmt::string::vector::ref t)
	{
		return [&](fmt::string::view u)
		{
			auto s = fmt::to_string(u);
			t.emplace_back(move(s));
			return success;
		};
	}

	entry to(fmt::string::ref s)
	{
		return [&](fmt::string::view u)
		{
			s = fmt::to_string(u);
			return success;
		};
	}

	bool fail(fmt::string::view path, file::mode am)
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

	fmt::string::view make(fmt::string::view path)
	{
		std::stack<fmt::string::view> stack;
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

	bool remove(fmt::string::view dir)
	{
		std::deque<fmt::string> deque;
		deque.emplace_back(dir);

		for (auto it = deque.begin(); it != deque.end(); ++it)
		{
			(void) find(*it, [&](fmt::string::view u)
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

#ifndef NDEBUG
void test::run<test::unit::dir>() noexcept
{
	auto const path = fmt::dir::split(__FILE__);
	assert(not empty(path));
	auto const name = path.back();
	assert(not empty(name));

	assert(env::dir::find(env::pwd, [&](auto entry)
	{
		return fmt::dir::split(entry).back() == name;
	}));

	auto const temp = fmt::dir::join({env::tmpdir, "my", "test", "dir"});
	auto const stem = env::dir::make(temp);
	assert(not empty(stem));
	assert(not env::dir::remove(stem));
}
#endif
