// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "arg.hpp"
#include "ini.hpp"
#include "str.hpp"
#include "type.hpp"
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

	fmt::string::view::vector arg_list;

	struct : env::span
	{

		operator type() const final
		{
			return { arg_list.data(), arg_list.size() };
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
			static fmt::string::view u;
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

	auto make_pair(env::opt::word key)
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
			auto writer = ini.write();
			fmt::string::view const path = env::opt::initials;
			auto const s = fmt::to_string(path);
			doc::ini::ref slice = *writer;
			slice.set(make_pair(app), env::opt::program);
			std::ifstream input(s);
			while (input >> slice);
			return ini;
		}
	}
}

namespace env::opt
{
	env::view::ref application = APPLICATION;
	env::span::ref arguments = ARGUMENTS;
	env::view::ref initials = INITIALS;
	env::view::ref program = PROGRAM;
	env::view::ref config = CONFIG;

	string::in::ref get(string::in::ref input)
	{
		auto writer = registry().write();
		doc::ini::ref slice = *writer;
		return input >> slice;
	}

	string::out::ref put(string::out::ref output)
	{
		auto const reader = registry().read();
		doc::ini::cref slice = *reader;
		return output << slice;
	}

	bool got(pair key)
	{
		return registry().read()->got(key);
	}

	string::view get(pair key)
	{
		return registry().read()->get(key);
	}

	bool set(pair key, string::view value)
	{
		return registry().write()->set(key, value);
	}

	bool got(word key)
	{
		return not empty(get(key));
	}

	string::view get(word key)
	{
		auto const u = fmt::str::get(key);
		// First look for argument
		string::view::span const args = arguments;
		for (auto const a : args)
		{
			auto const e = fmt::to_pair(a);
			if (e.first == u)
			{
				return e.second;
			}
		}
		// Second look in environment
		auto value = env::var::get(u);
		if (empty(value))
		{
			// Finally look in options table
			value = env::opt::get(make_pair(key));
		}
		return value;
	}

	bool set(word key, string::view value)
	{
		return set(make_pair(key), value);
	}

	string::view::vector put(int argc, char** argv, commands cmd)
	{
		auto writer = registry().write();
		// Push a view to command line arguments
		auto push = std::back_inserter(arg_list);
		std::copy(argv, argv + argc, push);
		// Boundary of command line
		auto const begin = cmd.begin();
		auto const end = cmd.end();
		// Command iterators
		auto next = end;
		auto it = end;

		string::view::pair entry;
		string::view::vector args, extra;

		for (int argn = 0; argv[argn]; ++argn)
		{
			string::view const argu = argv[argn];

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
					auto const name = fmt::str::put(it->name);
					auto const key = make_pair(name);
					if (empty(args))
					{
						writer->set(key, entry.second);
					}
				}

				it = next;
				next = end;
				argn = it->argn;

				if (0 == argn)
				{
					auto const name = fmt::str::put(it->name);
					auto const key = make_pair(name);
					auto const value = doc::ini::join(args);
					writer->set(key, value);
				}
			}
		}

		if (end != it)
		{
			auto const name = fmt::str::put(it->name);
			auto const key = make_pair(name);
			if (empty(args))
			{
				writer->set(key, entry.second);
			}
			else
			{
				auto const value = doc::ini::join(args);
				writer->set(key, value);
			}
		}

		return extra;
	}
}
