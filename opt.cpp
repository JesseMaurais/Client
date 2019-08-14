#include "opt.hpp"
#include "usr.hpp"
#include "dir.hpp"
#include "ini.hpp"
#include "fmt.hpp"
#include "int.hpp"
#include "err.hpp"
#include "sys.hpp"
#include <algorithm>
#include <iterator>
#include <fstream>
#include <cmath>
#include <set>

#ifdef _WIN32
# include "win.hpp"
#else
# include "uni.hpp"
#endif

namespace
{
	auto extension(fmt::string_view base)
	{
		return fmt::join({ base, ".ini" });
	}

	struct : env::span
	{
		fmt::string_view_vector list;

		operator fmt::string_view_span() const final
		{
			return list;
		}

	} ARGUMENTS;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			static fmt::string_view u;
			if (empty(u))
			{
				fmt::string_view_span args = env::opt::arguments;
				assert(not empty(args));
				auto const path = args.front();
				assert(not empty(path));
				auto const dirs = fmt::dir::split(path);
				assert(not empty(dirs));
				auto const name = dirs.back();
				assert(not empty(name));
				auto const first = name.find_first_not_of("./");
				auto const last = name.rfind(sys::ext::image);
				u = name.substr(first, last);
			}
			return u;
		}

	} PROGRAM;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			static fmt::string s;
			if (empty(s))
			{
				auto home = env::opt::directory(env::usr::config_home);
				if (env::dir::fail(home))
				{
					fmt::string_view_span dirs = env::usr::config_dirs;
					for (auto const dir : dirs)
					{
						s = env::opt::directory(dir);
						if (env::dir::fail(s))
						{
							continue;
						}
						return s;
					}
					s = move(home);
				}
			}
			return s;
		}

	} CONFIG;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			static fmt::string s;
			if (empty(s))
			{
				s = env::opt::directory(env::usr::cache_home);
			}
			return s;
		}

	} CACHE;

	auto open()
	{
		struct config : ini
		{
			~config()
			{
				auto const base = env::opt::directory(env::usr::config_home);
				auto const path = extension(base);
				std::ofstream file(path);
				file << env::opt::put;
			}

		} keys;

		auto const path = extension(env::opt::config);
		std::ifstream file(path);
		file >> keys;
		return keys;
	}

	ini & registry()
	{
		static auto keys = open();
		return keys;
	}

	sys::rwlock lock;

	auto make_pair(env::opt::view key)
	{
		return make_pair("Defaults", key);
	}

	template <typename K, typename V, typename C>
	V convert(K key, V value, C converter)
	{
		auto const u = env::opt::get(key);
		auto const n = converter(u);
		return fmt::fail(n) ? value : n;
	}

	template <typename K>
	bool convert(K key, bool value)
	{
		auto const check = { "0", "no", "off", "disable" };
		auto const u = env::opt::get(key);
		if (not empty(u))
		{
			for (auto const v : check)
			{
				if (u.starts_with(v))
				{
					return false;
				}
			}
			return true;
		}
		return value;
	}
}

namespace env::opt
{
	env::span const& arguments = ARGUMENTS;
	env::view const& program = PROGRAM;
	env::view const& config = CONFIG;
	env::view const& cache = CACHE;

	view directory(view stem)
	{
		return fmt::dir::join(stem, application);
	}

	void set(int argc, char** argv)
	{
		auto const unlock = lock.write();
		auto back = back_inserter(ARGUMENTS.list);
		copy(argv, argv + argc, back);
	}

	list arg(view key, int n)
	{
		auto const unlock = lock.read();
		auto const begin = ARGUMENTS.list.begin();
		auto const end = ARGUMENTS.list.end();
		list items;
		for (auto it = find(begin, end, key); it != end; ++it)
		{
			if (begin != it)
			{
				if (it->starts_with("--"))
				{
					break;
				}
				else
				if (n == 0)
				{
					break;
				}
				else --n;
			}
			items.push_back(*it);
		}
		return items;
	}

	view get(view key)
	{
		fmt::string_view_span args = env::opt::arguments;
		for (auto const arg : args)
		{
			const auto e = fmt::entry(arg);
			if (e.first == key)
			{
				return e.second;
			}
		}
		
		auto value = sys::env::get(key);
		if (empty(value))
		{
			auto const entry = make_pair(key);
			value = env::opt::get(entry);
		}
		return value;
	}

	bool set(view key, view value)
	{
		auto const entry = make_pair(key);
		return set(entry, value);
	}

	bool put(view key, view value)
	{
		auto const entry = make_pair(key);
		return put(entry, value);
	}

	bool cut(view key)
	{
		auto const entry = make_pair(key);
		return cut(entry);
	}

	view get(pair key)
	{
		auto const unlock = lock.read();
		std::set<view> set;
		while (true)
		{
			view value = registry().get(key);
			if (value.front() != '@')
			{
				return value;
			}
			if (not set.emplace(value).second)
			{
				sys::warn(here, "Cycle detected at", value);
				break;
			}
			key.second = value.substr(1);
		}
		return fmt::nil;
	}

	bool set(pair key, view value)
	{
		auto const unlock = lock.write();
		return registry().set(key, value);
	}

	bool put(pair key, view value)
	{
		auto const unlock = lock.write();
		return registry().put(key, value);
	}

	bool cut(pair key)
	{
		auto const unlock = lock.write();
		return registry().cut(key);
	}

	std::istream & get(std::istream & in)
	{
		auto const unlock = lock.write();
		return in >> registry();
	}

	std::ostream & put(std::ostream & out)
	{
		auto const unlock = lock.read();
		return out << registry();
	}

	bool get(view key, bool value)
	{
		return convert(key, value);
	}

	bool get(pair key, bool value)
	{
		return convert(key, value);
	}

	bool put(pair key, bool value)
	{
		auto const unlock = lock.write();
		if (value)
		{
			return registry().set(key, "enable");
		}
		else
		{
			return registry().cut(key);
		}
	}

	bool put(view key, bool value)
	{
		auto const entry = make_pair(key);
		return put(entry, value);
	}

	word get(view key, word value, int base)
	{
		return convert(key, value, [base](auto value)
		{
			return fmt::to_llong(value, base);
		});
	}

	word get(pair key, word value, int base)
	{
		return convert(key, value, [base](auto value)
		{
			return fmt::to_llong(value, base);
		});
	}

	bool put(view key, word value, int base)
	{
		return put(key, fmt::to_string(value, base));
	}

	bool put(pair key, word value, int base)
	{
		return put(key, fmt::to_string(value, base));
	}

	quad get(view key, quad value)
	{
		return convert(key, value, [](auto value)
		{
			return fmt::to_quad(value);
		});
	}

	quad get(pair key, quad value)
	{
		return convert(key, value, [](auto value)
		{
			return fmt::to_quad(value);
		});
	}

	bool put(view key, quad value)
	{
		return set(key, fmt::to_string(value));
	}

	bool put(pair key, quad value)
	{
		return set(key, fmt::to_string(value));
	}
}

