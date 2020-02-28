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

namespace
{
	struct : env::pair
	{
		operator fmt::pair_view_span() const final
		{
			return { env::pwd, env::paths };
		}

	} PATHS;

	struct : env::pair
	{
		operator fmt::pair_view_span() const final
		{
			return { env::usr::config_home, env::usr::config_dirs };
		}

	} CONFIG;

	struct : env::pair
	{
		operator fmt::pair_view_span() const final
		{
			return { env::usr::data_home, env::usr::data_dirs };
		}

	} DATA;
}

namespace fmt::dir
{
	string join(span_view p)
	{
		return fmt::join(p, sys::sep::dir);
	}

	string join(list_view p)
	{
		return fmt::join(p, sys::sep::dir);
	}

	vector_view split(string_view u)
	{
		return fmt::split(u, sys::sep::dir);
	}
}

namespace fmt::path
{
	string join(span_view p)
	{
		return fmt::join(p, sys::sep::path);
	}

	string join(list_view p)
	{
		return fmt::join(p, sys::sep::path);
	}

	vector_view split(string_view u)
	{
		return fmt::split(u, sys::sep::path);
	}
}

namespace env::dir
{
	env::pair const& paths = PATHS;
	env::pair const& config = CONFIG;
	env::pair const& data = DATA;

	using namespace ::env;
	using namespace ::env::usr;

	bool find(fmt::string_view path, entry look)
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

	bool find(fmt::pair_view_span pair, entry look)
	{
		return find(pair.first, look) or find(pair.second, look);
	}

	entry mask(file::mode am)
	{
		auto const flags = file::convert(am);
		return [=](fmt::string_view u)
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

	entry regx(fmt::string_view u)
	{
		auto const s = fmt::to_string(u);
		auto const x = std::regex(s);
		return [x](fmt::string_view u)
		{
			std::cmatch cm;
			auto const s = fmt::to_string(u);
			return std::regex_search(s.data(), cm, x);
		};
	}

	entry to(fmt::vector_string& t)
	{
		return [&](fmt::string_view u)
		{
			auto s = fmt::to_string(u);
			t.emplace_back(move(s));
			return success;
		};
	}

	entry to(fmt::string &s)
	{
		return [&](fmt::string_view u)
		{
			s = fmt::to_string(u);
			return success;
		};
	}

	bool fail(fmt::string_view path, file::mode am)
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

	fmt::string_view make(fmt::string_view path)
	{
		std::stack<fmt::string_view> stack;
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

	bool remove(fmt::string_view dir)
	{
		std::deque<fmt::string> deque;
		deque.emplace_back(dir);

		for (auto it = deque.begin(); it != deque.end(); ++it)
		{
			(void) find(*it, [&](fmt::string_view u)
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
