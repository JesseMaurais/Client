// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "arg.hpp"
#include "ini.hpp"
#include "dir.hpp"
#include "sync.hpp"
#include "err.hpp"
#include "tag.hpp"
#include "type.hpp"
#include <fstream>

namespace
{
	fmt::vector list;

	fmt::view make_key()
	{
		static const fmt::view app = "Application";
		return app;
	}

	fmt::pair make_pair(fmt::view key = make_key())
	{
		static const fmt::view cmd = "Command Line";
		return std::make_pair(cmd, key);
	}

	fmt::string make_ini()
	{
		fmt::vector path { env::opt::program(), "ini" };
		return fmt::join(path, ".");
	}

	auto find_next(fmt::view argu, env::opt::command::span cmd)
	{
		const auto begin = cmd.begin();
		const auto end = cmd.end();
		auto next = end;

		if (auto entry = argu.substr(2); argu.starts_with(fmt::tag::dual))
		{
			next = std::find_if
			(
				begin, end, [entry](auto const& d)
				{
					return d.name == entry;
				}
			);
		}
		else
		if (auto entry = argu.substr(1); argu.starts_with(fmt::tag::dash))
		{
			next = std::find_if
			(
				begin, end, [entry](auto const& d)
				{
					return d.dash == entry;
				}
			);
		}

		return next;
	}

	auto& registry()
	{
		static sys::exclusive<doc::ini> ini;
		// try read
		{
			auto const reader = ini.reader();
			if (not reader->keys.empty())
			{
				return ini;
			}
		}
		// next write
		{
			auto writer = ini.writer();
			auto const path = env::opt::initials();
			auto const s = fmt::to_string(path);
			doc::ini::ref slice = *writer;
			slice.set(make_pair(), env::opt::program());
			std::ifstream input(s);
			while (input >> slice);
			return ini;
		}
	}
}

namespace env::opt
{
	view application()
	{
		return env::opt::get(make_key());
	}

	span arguments()
	{
		return { list.data(), list.size() };
	}

	view initials()
	{
		static fmt::string s;
		if (empty(s))
		{
			s = fmt::dir::join({config(), make_ini()});
		}
		return s;
	}

	view program()
	{
		static fmt::string::view u;
		if (u.empty())
		{
			auto const args = env::opt::arguments();
			assert(not args.empty());
			auto const path = args.front();
			assert(not path.empty());
			auto const dirs = fmt::dir::split(path);
			assert(not dirs.empty());
			auto const name = dirs.back();
			assert(not name.empty());
			auto const first = name.find_first_not_of("./");
			auto const last = name.rfind(sys::ext::image);
			u = name.substr(first, last);
		}
		return u;
	}

	view config()
	{
		static fmt::string s;
		if (s.empty())
		{
			auto const filename = make_ini();
			for (auto dirs : { env::file::config(), env::file::paths() })
			{
				using namespace env::file;
				if (find(dirs, regx(filename) || to(s) || stop))
				{
					break;
				}
			}
		}
		return s;
	}

	fmt::string::in::ref get(fmt::string::in::ref in)
	{
		auto writer = registry().writer();
		doc::ini::ref slice = *writer;
		return in >> slice;
	}

	fmt::string::out::ref put(fmt::string::out::ref out)
	{
		auto reader = registry().reader();
		doc::ini::cref slice = *reader;
		return out << slice;
	}

	bool got(pair key)
	{
		return registry().reader()->got(key);
	}

	view get(pair key)
	{
		return registry().reader()->get(key);
	}

	bool set(pair key, view value)
	{
		return registry().writer()->set(key, value);
	}

	bool got(view key)
	{
		return not get(key).empty();
	}

	view get(view key)
	{
		const auto u = fmt::tag::get(key);
		// First look for argument
		const auto args = arguments();
		for (const auto a : args)
		{
			const auto e = fmt::to_pair(a);
			if (e.first == u)
			{
				return e.second;
			}
		}
		// Second look in environment
		auto value = env::get(u);
		if (value.empty())
		{
			// Finally look in options table
			value = env::opt::get(make_pair(key));
		}
		return value;
	}

	bool set(fmt::view key, fmt::view value)
	{
		return set(make_pair(key), value);
	}

	fmt::string::view::vector put(int argc, char** argv, command::span cmd)
	{
		assert(nullptr == argv[argc]);
		// Push a view to command line arguments
		std::copy(argv, argv + argc, std::back_inserter(list));
		// Arguments not part of a command
		fmt::string::view::vector extra;
		fmt::string::view::vector args;
		// Command line range
		auto const end = cmd.end();
		auto current = end;
		// Skip the program image path
		for (int argn = 1; argn < argc; ++argn)
		{
			fmt::string::view argu(argv[argn]);
			// Check whether it is a new command
			if (auto next = find_next(argu, cmd); end != next)
			{
				// Set as option
				auto const key = fmt::tag::set(next->name);
				current = 0 < next->argn ? next : end;
				env::opt::set(key, true);
				args.clear();
			}
			else
			if (end != current)
			{
				auto const size = static_cast<size_t>(current->argn);
				if (args.size() < size)
				{
					// Set as option
					args.emplace_back(argu);
					auto const value = fmt::join(args, ";");
					auto const key = fmt::tag::set(current->name);
					(void) set(key, value);
				}
				else
				{
					// Accept no more
					current = end;
					args.clear();
				}
			}
			else
			{
				extra.emplace_back(argu);
			}
		}
		return extra;
	}
}

#ifdef test_unit
test_unit(arg)
{
	// Application name exists
	{
		const auto app = env::opt::application();
		assert(not app.empty() and "Application is not named");
		assert(env::opt::arg().find(app) != fmt::npos);
	}
	// Dump options into stream
	{
		std::stringstream ss;
		ss << env::opt::put;
		const auto s = ss.str();
		assert(not s.empty() and "Cannot dump options");
	}
}
#endif
