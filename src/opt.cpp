// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "err.hpp"
#include "env.hpp"
#include "opt.hpp"
#include "arg.hpp"
#include "fmt.hpp"
#include "dig.hpp"
#include "ini.hpp"
#include "dir.hpp"
#include "type.hpp"
#include "sync.hpp"
#include <fstream>

namespace
{
	template
	<
		class Key, class Value, class Cast
	>
	auto cast(Key key, Value value, Cast cast)
	{
		const auto u = env::opt::get(key);
		return u.empty() ? value : cast(u);
	}

	auto cast(bool value)
	{
		return value ? "true" : "false";
	}

	template <class Key> bool cast(Key key, bool value)
	{
		const auto check = { cast(false), "0", "no", "off", "disable" };
		const auto u = env::opt::get(key);
		if (not u.empty())
		{
			const auto s = fmt::to_lower(u);
			for (const auto v : check)
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

namespace fmt::opt
{
	pair split(view param, pair style)
	{
		auto [key, value] = to_pair(param, style.second);
		if (key.starts_with(style.first))
		{
			const auto pos = style.first.size();
			key = key.substr(pos);
		}
		value = trim(value, tag::quote);
		return { key, value };
	}

	string join(pair param, pair style)
	{
		buffer buf;
		auto key = trim(param.first, tag::quote);
		auto value = trim(param.second, tag::quote);
		buf << style.first << key;
		if (not value.empty())
		{
			buf << style.second;
			if (any_of(value, space))
			{
				buf << tag::quote << value << tag::quote;
			}
			else buf << value;
		}
		return buf.str();
	}
}

namespace env::opt
{
	bool get(fmt::view key, bool value)
	{
		return cast(key, value);
	}

	bool set(fmt::view key, bool value)
	{
		fmt::view u = cast(value);
		return set(key, u);
	}

	bool get(fmt::pair key, bool value)
	{
		return cast(key, value);
	}

	bool set(fmt::pair key, bool value)
	{
		fmt::view u = cast(value);
		return set(key, u);
	}

	fmt::view get(fmt::view key, fmt::view value)
	{
		return got(key) ? get(key) : value;
	}

	fmt::view get(fmt::pair key, fmt::view value)
	{
		return got(key) ? get(key) : value;
	}

	long get(fmt::view key, long value, int base)
	{
		return cast(key, value, [base](auto value)
		{
			return fmt::to_long(value, base);
		});
	}

	bool set(fmt::view key, long value, int base)
	{
		return set(key, fmt::to_string(value, base));
	}

	long get(fmt::pair key, long value, int base)
	{
		return cast(key, value, [base](auto value)
		{
			return fmt::to_long(value, base);
		});
	}

	bool set(fmt::pair key, long value, int base)
	{
		return set(key, fmt::to_string(value, base));
	}

	float get(fmt::view key, float value)
	{
		return cast(key, value, [](auto value)
		{
			return fmt::to_float(value);
		});
	}

	bool set(fmt::view key, float value, int digits)
	{
		return set(key, fmt::to_string(value, digits));
	}

	float get(fmt::pair key, float value)
	{
		return cast(key, value, [](auto value)
		{
			return fmt::to_float(value);
		});
	}

	bool set(fmt::pair key, float value)
	{
		return set(key, fmt::to_string(value));
	}
}


namespace
{
	fmt::vector list;

	constexpr fmt::view Application = "Application";
	constexpr fmt::pair CommandLine { Application, "Command Line" };

	fmt::string make_ini()
	{
		fmt::vector path { env::opt::program(), "ini" };
		return fmt::join(path, ".");
	}

	auto find_next(fmt::view argu, env::opt::cmd::span cmd)
	{
		auto next = cmd.end();

		if (auto entry = argu.substr(2); argu.starts_with(fmt::tag::dual))
		{
			next = fwd::find_if
			(
				cmd, [entry](auto const& d)
				{
					return d.name == entry;
				}
			);
		}
		else
		if (auto entry = argu.substr(1); argu.starts_with(fmt::tag::dash))
		{
			next = fwd::find_if
			(
				cmd, [entry](auto const& d)
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
			slice.set(CommandLine, env::opt::program());
			std::ifstream input(s);
			while (input >> slice);
			return ini;
		}
	}
}

namespace env::opt
{
	fmt::view application()
	{
		return env::opt::get(CommandLine);
	}

	fmt::span arguments()
	{
		return { list.data(), list.size() };
	}

	fmt::view initials()
	{
		static fmt::string s;
		if (s.empty())
		{
			s = fmt::dir::join({config(), make_ini()});
		}
		return s;
	}

	fmt::view program()
	{
		static fmt::view u;
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
			auto const last = name.rfind(sys::tag::image);
			u = name.substr(first, last);
		}
		return u;
	}

	fmt::view config()
	{
		static fmt::string s;
		if (s.empty())
		{
			auto const filename = make_ini();
			for (auto dirs : { env::file::config(), env::file::paths() })
			{
				using namespace env::file;
				if (find(dirs, regex(filename) || to(s) || stop))
				{
					break;
				}
			}
		}
		return s;
	}

	fmt::view catalog()
	{
		auto u = get("catalog");
		if (u.empty())
		{
			u = program();
		}
		return u;
	}

	fmt::input get(fmt::input in)
	{
		auto writer = registry().writer();
		auto &slice = *writer;
		return in >> slice;
	}

	fmt::output put(fmt::output out)
	{
		auto reader = registry().reader();
		auto &slice = *reader;
		return out << slice;
	}

	bool got(fmt::pair key)
	{
		return registry().reader()->got(key);
	}

	fmt::view get(fmt::pair key)
	{
		return registry().reader()->get(key);
	}

	bool set(fmt::pair key, fmt::view value)
	{
		return registry().writer()->set(key, value);
	}

	bool got(fmt::view key)
	{
		return not get(key).empty();
	}

	fmt::view get(fmt::view key)
	{
		const auto u = fmt::tag::emplace(key);
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
			value = env::opt::get({Application, key});
		}
		return value;
	}

	bool set(fmt::view key, fmt::view value)
	{
		return set({Application, key}, value);
	}

	fmt::vector parse(int argc, char** argv, cmd::span cmd)
	{
		#ifdef assert
		assert(nullptr == argv[argc]);
		#endif
		// Push a view to command line arguments
		std::copy(argv, argv + argc, std::back_inserter(list));
		// Arguments not part of a command
		fmt::vector extra, args;
		// Command line range
		const auto end = cmd.end();
		auto current = end;
		// Skip the program image path
		for (int argn = 1; argn < argc; ++argn)
		{
			fmt::view argu(argv[argn]);
			// Check whether it is a new command
			if (auto next = find_next(argu, cmd); end != next)
			{
				// Set as option
				const auto key = fmt::tag::emplace(next->name);
				current = 0 < next->argn ? next : end;
				env::opt::set(key, true);
				args.clear();
			}
			else
			if (end != current)
			{
				const auto size = fmt::to_size(current->argn);
				if (args.size() < size)
				{
					// Set as option
					args.emplace_back(argu);
					const auto value = fmt::join(args, ";");
					const auto key = fmt::tag::emplace(current->name);
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
		assert(env::opt::arg(0).find(app) != fmt::npos);
	}
	// Dump options into stream
	{
		fmt::buffer buf;
		buf << env::opt::put;
		const auto s = buf.str();
		assert(not s.empty() and "Cannot dump options");
	}
}
#endif
