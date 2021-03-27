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
		static auto const app = fmt::str::put("Application");
		return app;
	}

	auto make_pair(env::opt::name key = make_key())
	{
		static auto const cmd = fmt::str::set("Command Line");
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

	class strings : fwd::unique
	{
		strings() = default;

		sys::exclusive<fmt::string::set> cache;
		sys::exclusive<fmt::string::view::vector> store;
		sys::exclusive<std::map<fmt::string::view, env::opt::name>> table;

	public:

		bool got(env::opt::name key)
		{
			env::opt::name const index = ~key;
			std::ptrdiff_t const size = store.read()->size();
			return -1 < index and index < size;
		}

		bool got(fmt::string::view name)
		{
			auto const reader = table.read();
			auto const it = reader->find(name);
			auto const end = reader->end();
			return end != it;
		}

		fmt::string::view get(env::opt::name key)
		{
			auto const reader = store.read();
			auto const index = fmt::to_size(~key);
			assert(got(key) and "String is not stored");
			return reader->at(index);
		}

		fmt::string::view get(fmt::string::view key)
		{
			assert(not empty(key));
			// Lookup
			{
				auto const reader = table.read();
				auto const it = reader->find(key);
				auto const end = reader->end();
				if (end != it)
				{
					return it->first;
				}
			}
			// Create
			return get(set(key));
		}

		env::opt::name put(fmt::string::view key)
		{
			assert(not empty(key));
			// Lookup
			{
				auto const reader = table.read();
				auto const end = reader->end();
				auto const it = reader->find(key);
				if (end != it)
				{
					return ~it->second;
				}
			}
			// Create
			auto writer = store.write();
			auto const size = writer->size();
			auto const id = fmt::to<env::opt::name>(size);
			{
				auto [it, unique] = table.write()->emplace(key, id);
				assert(unique);
				writer->push_back(it->first);
			}
			return ~id;
		}

		env::opt::name set(fmt::string::view key)
		{
			// Lookup
			{
				auto const reader = table.read();
				auto const end = reader->end();
				auto const it = reader->find(key);
				if (end != it)
				{
					return ~it->second;
				}
			}
			// Create
			auto writer = store.write();
			auto const size = writer->size();
			auto const id = fmt::to<env::opt::name>(size);
			{
				// Cache the string here
				auto const p = cache.write()->emplace(key);
				verify(p.second);
				// Index a view to the string
				auto const q = table.write()->emplace(*p.first, id);
				verify(q.second);
				
				writer->push_back(q.first->first);
			}
			return ~id;
		}

		fmt::string::in::ref get(fmt::string::in::ref in, char end)
		{
			// Block all threads at this point
			auto wcache = cache.write();
			auto wstore = store.write();
			auto wtable = table.write();

			fmt::string line;
			while (std::getline(in, line, end))
			{
				auto const p = wcache->emplace(move(line));
				assert(p.second);

				auto const size = wstore->size();
				auto const id = fmt::to<env::opt::name>(size);	
				wstore->emplace_back(*p.first);

				auto const q = wtable->emplace(*p.first, id);
				assert(q.second);
			}
			return in;
		}

		fmt::string::out::ref put(fmt::string::out::ref out, char eol)
		{
			auto const reader = cache.read();
			auto const begin = reader->begin();
			auto const end = reader->end();

			for (auto it = begin; it != end; ++it)
			{
				out << *it << eol;
			}
			return out;
		}

		static auto& registry()
		{
			static strings singleton;
			return singleton;
		}
	};
}

namespace fmt::str
{
	bool got(name n)
	{
		return strings::registry().got(n);
	}

	bool got(view n)
	{
		return strings::registry().got(n);
	}

	view get(name n)
	{
		return strings::registry().get(n);
	}

	view get(view n)
	{
		return strings::registry().get(n);
	}

	name put(view n)
	{
		return strings::registry().put(n);
	}

	name set(view n)
	{
		return strings::registry().set(n);
	}

	string::in::ref get(string::in::ref in, char end)
	{
		return strings::registry().get(in, end);
	}

	string::out::ref put(string::out::ref out, char end)
	{
		return strings::registry().put(out, end);
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

	word get(name key, word value, int base)
	{
		return cast(key, value, [base](auto value)
		{
			return fmt::to_llong(value, base);
		});
	}

	bool set(name key, word value, int base)
	{
		return set(key, fmt::to_string(value, base));
	}

	word get(pair key, word value, int base)
	{
		return cast(key, value, [base](auto value)
		{
			return fmt::to_llong(value, base);
		});
	}

	bool set(pair key, word value, int base)
	{
		return set(key, fmt::to_string(value, base));
	}

	quad get(name key, quad value)
	{
		return cast(key, value, [](auto value)
		{
			return fmt::to_quad(value);
		});
	}

	bool set(name key, quad value, int digits)
	{
		return set(key, fmt::to_string(value, digits));
	}

	quad get(pair key, quad value)
	{
		return cast(key, value, [](auto value)
		{
			return fmt::to_quad(value);
		});
	}

	bool set(pair key, quad value)
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
		auto const u = fmt::str::get(key);
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
				auto const key = fmt::str::set(next->name);
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
					auto const key = fmt::str::set(current->name);
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