#include "opt.hpp"
#include "usr.hpp"
#include "dir.hpp"
#include "ini.hpp"
#include "fmt.hpp"
#include "dig.hpp"
#include "err.hpp"
#include "sys.hpp"
#include "thread.hpp"
#include <algorithm>
#include <iterator>
#include <fstream>
#include <cmath>
#include <set>

namespace
{
	struct : env::span
	{
		fmt::list_view list;

		operator fmt::span_view() const final
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
				fmt::span<fmt::view> const args = env::opt::arguments;
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
		operator fmt::view() const final
		{
			static fmt::string s;
			if (empty(s))
			{
				using namespace env::dir;
				env::dir::find(config, regx(".ini") || to(s) || stop));
			}
			return s;
		}

	} CONFIG;

	struct : env::view
	{
		operator fmt::view() const final
		{
			static auto const s = env::opt::directory(env::usr::run_dir);
			static auto const run = env::dir::tmp(s);
			return s;
		}

	} RUN;

	auto& open()
	{
		static doc::ini keys;
		fmt::view const path = env::opt::config;
		auto const s = fmt::to_string(path);
		std::ifstream file(s);
		if (file) file >> keys;
		return keys;
	}

	auto& registry()
	{
		static auto& keys = open();
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
		auto const check = { "0", "no", "off", "false", "disable" };
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
	env::view const& run = RUN;

	string directory(view base)
	{
		return fmt::dir::join({ base, application });
	}

	string initials(view base)
	{
		return fmt::dir::join({ base, application, ".ini" });
	}

	void set(int argc, char** argv)
	{
		auto const unlock = lock.write();
		auto back = back_inserter(ARGUMENTS.list);
		copy(argv, argv + argc, back);
	}

	list parse(commands const& cmd)
	{
		auto const unlock = lock.write();
		auto const begin = cmd.begin();
		auto const end = cmd.end();

		auto next = end;
		auto it = end;
		pair entry;

		word argn = 0;
		list args;
		list extra;

		span argv = arguments;
		for (auto const arg : argv)
		{
			// Check whether this argument is a new command

			if (arg.starts_with("--"))
			{
				entry = fmt::entry(arg.substr(2));
				next = find_if(begin, end, [&](auto const& desc)
				{
					return desc.name == entry.first;
				});
			}
			else
			if (arg.starts_with("-"))
			{
				entry = fmt::entry(arg.substr(1));
				next = find_if(begin, end, [&](auto const& desc)
				{
					return desc.nick == entry.first;
				});
			}

			bool const change = (it != next) and (next != end);

			// Push it either to the command list or as an extra

			if (not change)
			{
				if (0 != argn)
				{
					--argn;
					args.push_back(arg);
				}
				else
				if (not fmt::same(argv[0], arg))
				{
					extra.push_back(arg);
				}
			}

			// Start parsing the next command if changing

			if (change)
			{
				if (end != it)
				{
					auto const key = make_pair(it->name);
					if (empty(args))
					{
						(void) registry().put(key, entry.second);
					}
				}

				it = next;
				next = end;
				argn = it->argn;

				if (0 == argn)
				{
					auto const key = make_pair(it->name);
					auto const value = doc::ini::join(args);
					(void) registry().put(key, value);
				}
			}
		}

		if (end != it)
		{
			auto const key = make_pair(it->name);
			if (empty(args))
			{
				(void) registry().put(key, entry.second);
			}
			else
			{
				auto const value = doc::ini::join(args);
				(void) registry().put(key, value);
			}
		}

		return extra;
	}

	view get(view key)
	{
		span const args = env::opt::arguments;
		for (auto const arg : args)
		{
			auto const e = fmt::entry(arg);
			if (e.first == key)
			{
				return e.second;
			}
		}

		auto value = env::var::get(key);
		if (empty(value))
		{
			auto const entry = make_pair(key);
			value = env::opt::get(entry);
		}
		return value;
	}

	bool got(view key)
	{
		auto const entry = make_pair(key);
		return got(entry);
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
				sys::warn(here, "Cycle", value);
				break;
			}
			key.second = value.substr(1);
		}
		return fmt::nil;
	}

	bool got(pair key)
	{
		auto const unlock = lock.read();
		return registry().got(key);
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

	span get(view key, span value)
	{
		auto const entry = make_pair(key);
		return get(entry, value);
	}

	span get(pair key, span value)
	{
		view u = get(key);
		if (empty(u))
		{
			return value;
		}
		return doc::ini::split(u);
	}

	bool put(view key, span value)
	{
		auto const entry = make_pair(key);
		return put(entry, value);
	}

	bool put(pair key, span value)
	{
		auto const s = doc::ini::join(value);
		return put(key, s);
	}
}
