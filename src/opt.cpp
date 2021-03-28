// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "opt.hpp"
#include "arg.hpp"
#include "ini.hpp"
#include "dir.hpp"
#include "str.hpp"
#include "fmt.hpp"
#include "dig.hpp"
#include "type.hpp"
#include "sync.hpp"

namespace
{
	fmt::string::view::vector list;

	auto make_key()
	{
		static auto const app = fmt::put("Application");
		return app;
	}

	auto make_pair(env::opt::name key = make_key())
	{
		static auto const cmd = fmt::set("Command Line");
		return std::make_pair(cmd, key);
	}

	auto make_ini()
	{
		return fmt::join({env::opt::program(), "ini"}, ".");
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
			auto const path = env::opt::initials();
			auto const s = fmt::to_string(path);
			doc::ini::ref slice = *writer;
			slice.set(make_pair(), env::opt::program());
			std::ifstream input(s);
			while (input >> slice);
			return ini;
		}
	}

	auto find_next(fmt::string::view argu, env::opt::command::span cmd)
	{
		auto const begin = cmd.begin();
		auto const end = cmd.end();
		auto next = end;

		if (argu.starts_with(env::opt::dual))
		{
			auto const entry = argu.substr(2);
			next = std::find_if
			(
				begin, end, [entry](auto const& d)
				{
					return d.name == entry;
				}
			);
		}
		else
		if (argu.starts_with(env::opt::dash))
		{
			auto const entry = argu.substr(1);
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

	template
	<
		class Key, class Value, class Cast
	>
	auto cast(Key key, Value value, Cast cast)
	{
		auto const u = env::opt::get(key);
		return empty(u) ? value : cast(u);
	}

	auto cast(bool value)
	{
		return value ? "true" : "false";
	}

	template <class Key> bool cast(Key key, bool value)
	{
		auto const check = { cast(false), "0", "no", "off", "disable" };
		auto const u = env::opt::get(key);
		if (not empty(u))
		{
			auto const s = fmt::to_lower(u);
			for (auto const v : check)
			{
				if (s.starts_with(v))
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
	// opt.hpp

	bool get(name key, bool value)
	{
		return cast(key, value);
	}

	bool set(name key, bool value)
	{
		fmt::string::view u = cast(value);
		return set(key, u);
	}

	bool get(pair key, bool value)
	{
		return cast(key, value);
	}

	bool set(pair key, bool value)
	{
		fmt::string::view u = cast(value);
		return set(key, u);
	}

	view get(name key, view value)
	{
		return got(key) ? get(key) : value;
	}

	view get(pair key, view value)
	{
		return got(key) ? get(key) : value;
	}

	long get(name key, long value, int base)
	{
		return cast(key, value, [base](auto value)
		{
			return fmt::to_long(value, base);
		});
	}

	bool set(name key, long value, int base)
	{
		return set(key, fmt::to_string(value, base));
	}

	long get(pair key, long value, int base)
	{
		return cast(key, value, [base](auto value)
		{
			return fmt::to_long(value, base);
		});
	}

	bool set(pair key, long value, int base)
	{
		return set(key, fmt::to_string(value, base));
	}

	float get(name key, float value)
	{
		return cast(key, value, [](auto value)
		{
			return fmt::to_float(value);
		});
	}

	bool set(name key, float value, int digits)
	{
		return set(key, fmt::to_string(value, digits));
	}

	float get(pair key, float value)
	{
		return cast(key, value, [](auto value)
		{
			return fmt::to_float(value);
		});
	}

	bool set(pair key, float value)
	{
		return set(key, fmt::to_string(value));
	}

	// arg.hpp

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
		if (empty(u))
		{
			auto const args = env::opt::arguments();
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

	view config()
	{
		static fmt::string s;
		if (empty(s))
		{
			auto const filename = make_ini();
			for (auto const dirs : { env::file::config(), env::file::paths() })
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
		auto writer = registry().write();
		doc::ini::ref slice = *writer;
		return in >> slice;
	}

	fmt::string::out::ref put(fmt::string::out::ref out)
	{
		auto const reader = registry().read();
		doc::ini::cref slice = *reader;
		return out << slice;
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

	bool got(name key)
	{
		return not empty(get(key));
	}

	view get(name key)
	{
		auto const u = fmt::get(key);
		// First look for argument
		auto const args = arguments();
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

	bool set(name key, fmt::string::view value)
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
				auto const key = fmt::set(next->name);
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
					auto const value = doc::ini::join(args);
					auto const key = fmt::set(current->name);
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
		auto const app = env::opt::application();
		assert(not empty(app) and "Application is not named");
		assert(env::opt::arg().find(app) != fmt::npos);
	}
	// Dump options into stream
	{
		fmt::string::stream ss;
		ss << env::opt::put;
		auto const s = ss.str();
		assert(not empty(s) and "Cannot dump options");
	}
}
#endif