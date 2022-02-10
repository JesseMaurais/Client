// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dir.hpp"
#include "type.hpp"
#include "env.hpp"
#include "usr.hpp"
#include "arg.hpp"
#include "sys.hpp"
#include "err.hpp"
#include "sync.hpp"
#ifdef _WIN32
#include "win/file.hpp"
#else
#include "uni/dirent.hpp"
#ifdef __linux__
#include "uni/notify.hpp"
#endif
#endif

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

namespace fmt::file
{
	string fifo(string::view name)
	{
		#ifdef _WIN32
		{
			return fmt::join({ R"(\\.\pipe\)", name });
		}
		#else // UNIX
		{
			const auto run = env::usr::run_dir();
			return fmt::dir::join({ run, name });
		}
		#endif // API
	}
}

namespace env::file
{
	fmt::string::view::edges paths()
	{
		return { env::var::pwd(), env::var::path() };
	}

	fmt::string::view::edges config()
	{
		return { env::usr::config_home(), env::usr::config_dirs() };
	}

	fmt::string::view::edges data()
	{
		return { env::usr::data_home(), env::usr::data_dirs() };
	}

	bool find(fmt::string::view path, entry check)
	{
		if (not fmt::terminated(path))
		{
			return find(fmt::to_string(path), check);
		}
		auto const c = path.data();

		return fwd::any_of(sys::files(c), check);
	}

	bool find(fmt::string::view::span paths, entry check)
	{
		return fwd::any_of(paths, [check](auto path)
		{
			return find(path, check);
		});
	}

	bool find(fmt::string::view::edges paths, entry look)
	{
		return find(paths.first, look) or find(paths.second, look);
	}

	entry mask(mode am)
	{
		return [am](fmt::string::view u)
		{
			return not env::file::fail(u, am);
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

	entry to(fmt::string::vector& t)
	{
		return [&](fmt::string::view u)
		{
			auto s = fmt::to_string(u);
			t.emplace_back(move(s));
			return success;
		};
	}

	entry to(fmt::string& s)
	{
		return [&](fmt::string::view u)
		{
			s = fmt::to_string(u);
			return success;
		};
	}

	entry all(fmt::string::view u, mode m, entry e)
	{
		return mask(m) || regx(u) || e;
	}

	entry any(fmt::string::view u, mode m, entry e)
	{
		return all(u, m, e);
	}

	fmt::string::view mkdir(fmt::string::view path)
	{
		std::stack<fmt::string::view> stack;
		fmt::string buf;

		auto folders = fmt::dir::split(path);
		auto stem = path;

		while (env::file::fail(stem))
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

	bool rmdir(fmt::string::view dir)
	{
		std::deque<fmt::string> deque;
		deque.emplace_back(dir);

		for (auto it = deque.begin(); it != deque.end(); ++it)
		{
			(void) find(*it, [&](fmt::string::view u)
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
			auto const c = dir.data();
			if (sys::fail(sys::rmdir(c)))
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
	assert(not env::file::fail(env::var::temp()));
	assert(not env::file::fail(env::var::pwd()));
	assert(not env::file::fail(env::var::home()));

	auto const path = fmt::dir::split(__FILE__);
	assert(not empty(path));
	auto const name = path.back();
	assert(not empty(name));
	auto const program = env::opt::program();
	assert(not empty(program));

	assert(env::file::find(env::var::pwd(), [program](auto entry)
	{
		return fmt::dir::split(entry).back() == program;
	}));

	auto const temp = fmt::dir::join({env::var::temp(), "my", "test", "dir"});
	if (std::empty(temp)) return;
	auto const stem = env::file::make_dir(temp);
//	assert(not empty(stem.first));
//	assert(not empty(stem.second));
	assert(not env::file::remove_dir(stem));
}
#endif