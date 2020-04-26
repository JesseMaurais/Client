#include "arg.hpp"
#include "ini.hpp"
#include "str.hpp"
#include "dir.hpp"
#include "usr.hpp"
#include "sys.hpp"
#include "sync.hpp"
#include <iterator>
#include <algorithm>

namespace
{
	auto const app = fmt::str::put("Application");

	struct : env::view
	{
		operator type() const final
		{
			return env::opt::get(app);
		}

	} APPLICATION;

	struct : env::span
	{
		fmt::string::view::vector list;

		operator type() const final
		{
			return list;
		}

	} ARGUMENTS;

	struct : env::view
	{
		operator type() const final
		{
			static auto const u = fmt::dir::join
			(
				{ env::opt::config, env::opt::application, ".ini" }
			);
			return u;
		}

	} INITIALS;

	struct : env::view
	{
		operator type() const final
		{
			static string::view u;
			if (empty(u))
			{
				fmt::string::view::span const args = env::opt::arguments;
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
		operator type() const final
		{
			static fmt::string s;
			if (empty(s))
			{
				using namespace env::dir;
				env::dir::find(config, regx(".ini") || to(s) || stop);
			}
			return s;
		}

	} CONFIG;

	auto make_pair(word key)
	{
		static auto const cmd = fmt::str::set("Command Line");
		return std::make_pair(cmd, key);
	}

	auto& registry()
	{
		static sys::exclusive<doc::ini> ini;
		// try read
		{
			auto const reader = ini.read();
			if (not empty(reader->keys))
			{
				return ini;
			}
		}
		// next write
		{
			auto const writer = ini.write();
			fmt::string::view const path = env::opt::initials;
			auto const s = fmt::to_string(path);
			doc::ini::ref slice = *writer;
			slice.put(make_pair(app), env::opt::program);
			std::ifstream input(s);
			while (input >> slice);
			return ini;
		}
	}
}

namespace env::opt
{
	env::span::ref application = APPLICATION;
	env::span::ref arguments = ARGUMENTS;
	env::view::ref initials = INITIALS;
	env::view::ref program = PROGRAM;
	env::view::ref config = CONFIG;
	env::view::ref rundir = RUNDIR;

	in::ref get(in::ref input)
	{
		auto const writer = registry().write();
		doc::ini::ref slice = *writer;
		return input >> slice;
	}

	out::ref put(out::ref output)
	{
		auto const reader = registry().read();
		doc::ini::ref slice = *reader;
		return output << slice;
	}

	bool got(word key)
	{
		return not empty(get(key));
	}

	view get(word key)
	{
		// First look for argument
		span const args = arguments;
		for (auto const a : args)
		{
			auto const e = fmt::to_pair(a);
			if (e.first == fmt::str::get(key))
			{
				return e.second;
			}
		}
		// Second look in environment
		auto value = env::var::get(key);
		if (empty(value))
		{
			// Finally look in options table
			value = env::opt::get(make_pair(key));
		}
		return value;
	}

	bool set(word key, view value)
	{
		return set(make_pair(key), value);
	}

	bool put(word key, view value)
	{
		return put(make_pair(key), value);
	}

	bool got(pair key)
	{
		return registry().read()->got(key);
	}

	view get(pair key)
	{
		return registry().read()->get(key);
	}

	bool set(pair key, view value)
	{
		return registry().write()->set(key, value);
	}

	bool put(pair key, view value)
	{
		return registry().write()->put(key, value);
	}

	vector put(int argc, char** argv, commands cmd)
	{
		auto const writer = registry().write();
		// Push a view to command line arguments
		auto push = std::back_inserter(ARGUMENTS.list);
		std::copy(argv, argv + argc, push);
		// Boundary of command line
		auto const begin = cmd.begin();
		auto const end = cmd.end();
		// Command iterators
		auto next = end;
		auto it = end;
		// Current at
		pair entry;

		vector args;
		vector extra;

		for (int argn = 0; argv[argn]; ++argn)
		{
			view const argu = argv[argn];

			// Check whether this argument is a new command

			if (argu.starts_with("--"))
			{
				entry = fmt::to_pair(argu.substr(2));
				next = std::find_if(begin, end, [&](auto const& d)
				{
					return d.name == entry.first;
				});
			}
			else
			if (argu.starts_with("-"))
			{
				entry = fmt::to_pair(argu.substr(1));
				next = std::find_if(begin, end, [&](auto const& d)
				{
					return d.dash == entry.first;
				});
			}

			bool const change = (it != next) and (next != end);

			// Push it either to the command list or as an extra

			if (not change)
			{
				if (0 != argn)
				{
					--argn;
					args.push_back(argu);
				}
				else
				if (not fmt::same(argv[0], argu))
				{
					extra.push_back(argu);
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
						writer->put(key, entry.second);
					}
				}

				it = next;
				next = end;
				argn = it->argn;

				if (0 == argn)
				{
					auto const key = make_pair(it->name);
					auto const value = doc::ini::join(args);
					writer->put(key, value);
				}
			}
		}

		if (end != it)
		{
			auto const key = make_pair(it->name);
			if (empty(args))
			{
				(void) ptr->put(key, entry.second);
			}
			else
			{
				auto const value = doc::ini::join(args);
				(void) ptr->put(key, value);
			}
		}

		return extra;
	}
}
