// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "err.hpp"
#include "dir.hpp"
#include "type.hpp"
#include "env.hpp"
#include "usr.hpp"
#include "arg.hpp"
#include "sys.hpp"
#include "err.hpp"
#include "sync.hpp"
#include "event.hpp"
#include <regex>
#include <stack>

namespace fmt::path
{
	vector split(view u)
	{
		return fmt::split(u, sys::tag::path);
	}
	
	string join(span p)
	{
		return fmt::join(p, sys::tag::path);
	}

	string join(init n)
	{
		return fmt::join(fwd::to_span(n));
	}
}

namespace fmt::dir
{
	vector split(view u)
	{
		return fmt::split(u, sys::tag::dir);
	}

	string join(span p)
	{
		return fmt::join(p, sys::tag::dir);
	}

	string join(init n)
	{
		return fmt::join(fwd::to_span(n));
	}
}

namespace fmt::file
{
	vector split(view u)
	{
		return fmt::split(u, tag::dot);
	}

	string join(span p)
	{
		return fmt::join(p, tag::dot);
	}

	string join(init n)
	{
		return fmt::join(fwd::to_span(n));
	}

	string join(div p)
	{
		auto part = fmt::span(p.second);
		auto last = fmt::path::join(part);
		p.first.emplace_back(last);
		part = fmt::span(p.first);
		return fmt::dir::join(part);
	}

	div path(view u)
	{
		auto d = fmt::dir::split(u);
		auto n = d.back();
		auto b = fmt::file::split(n);
		d.pop_back();
		return { d, b };
	}
}

namespace env::file
{
	order paths()
	{
		return { env::pwd(), env::path() };
	}

	order config()
	{
		return { env::usr::config_home(), env::usr::config_dirs() };
	}

	order data()
	{
		return { env::usr::data_home(), env::usr::data_dirs() };
	}

	bool find(view path, entry check)
	{
		if (not fmt::terminated(path))
		{
			const auto buf = fmt::to_string(path);
			return find(view(buf), check);
		}
		return fwd::any_of(sys::files(path.data()), check);
	}

	bool find(span paths, entry check)
	{
		return fwd::any_of(paths, [check](auto path)
		{
			return find(path, check);
		});
	}

	bool find(order paths, entry check)
	{
		return find(paths.first, check) or find(paths.second, check);
	}

	entry mask(mode mask)
	{
		return [mask](view u)
		{
			return not env::file::fail(u, mask);
		};
	}

	entry regex(view u)
	{
		const auto s = fmt::to_string(u);
		const auto x = std::regex(s);
		return [x](view u)
		{
			std::cmatch m;
			const auto s = fmt::to_string(u);
			return std::regex_search(s.data(), m, x);
		};
	}

	entry to(string::vector& t)
	{
		return [&](view u)
		{
			auto s = fmt::to_string(u);
			t.emplace_back(std::move(s));
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
		return mask(m) || regex(u) || e;
	}

	entry any(view u, mode m, entry e)
	{
		return all(u, m, e);
	}

	string search(view name, entry check, order roots)
	{
		auto dirs = fmt::dir::split(name);
		auto first = not fwd::equal_to(fmt::tag::dots);
		auto it = fwd::find_if(dirs, first);
		auto diff = std::distance(dirs.begin(), it);
		auto sub =  fmt::span(dirs.data() + diff, dirs.size() - diff);
		// Search predicate
		string buf;
		entry visit = [&](view root)
		{
			auto folders = fmt::dir::split(root);
			if (ptrdiff_t size = folders.size(); size > diff)
			 {
				// Compound path parts
				folders.resize(size - diff);
				auto put = std::back_inserter(folders);
				fwd::copy(sub, put);
				// Compose a full string
				buf = fmt::dir::join(folders);
				return check(buf);
			 }
			return false;
		};
		// Clear if not found
		if (not find(roots, visit))
		{
			buf.clear();
		}
		return buf;
	}

	view mkdir(view path)
	{
		std::stack<view> stack;
		string buf;

		auto folders = fmt::dir::split(path);
		auto stem = path;

		while (env::file::fail(stem))
		{
			stack.push(folders.back());
			folders.pop_back();

			buf = fmt::dir::join(folders);
			stem = buf;
		}

		stem = path.substr(0, path.find(sys::tag::dir, stem.size() + 1));

		while (not stack.empty())
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

	bool rmdir(view dir)
	{
		std::deque<string> deque;
		deque.emplace_back(dir);

		for (auto it = deque.begin(); it != deque.end(); ++it)
		{
			(void) find(*it, [&](view u)
			{
				auto const path = fmt::dir::join({*it, u});
				auto const c = path.data();
				struct sys::stats st(c);
				if (sys::fail(st.ok))
				{
					sys::err(here, "stat", c);
				}
				else
				if (S_ISDIR(st.st_mode))
				{
					if (u != "." and u != "..")
					{
						deque.emplace_back(std::move(path));
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
		while (not deque.empty())
		{
			dir = deque.back();
			const auto c = dir.data();
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
	assert(not env::file::fail(env::temp()));
	assert(not env::file::fail(env::pwd()));
	assert(not env::file::fail(env::home()));

	const auto path = fmt::dir::split(__FILE__);
	assert(not path.empty());
	const auto name = path.back();
	assert(not name.empty());
	const auto program = env::opt::program();
	assert(not program.empty());

	assert(env::file::find(env::pwd(), [program](auto entry)
	{
		return fmt::dir::split(entry).back() == program;
	}));

	const auto temp = fmt::dir::join({env::temp(), "my", "test", "dir"});
	if (temp.empty()) return;
	const auto stem = env::file::mkdir(temp);
//	assert(not empty(stem.first));
//	assert(not empty(stem.second));
	assert(not env::file::rmdir(stem));
}
test_unit(dir)
{
	fmt::view dir, name, path = __FILE__;
	auto pos = path.find_last_of(sys::tag::dir);
	if (auto pair = path.split(pos); pair.second.empty()) 
	{
		dir = ".";
		name = pair.frist;
	}
	else
	{
		dir = pair.first;
		name = pair.second;
	}
}
#endif