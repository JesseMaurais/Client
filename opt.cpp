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

#ifdef _WIN32
# include "win.hpp"
#else
# include "uni.hpp"
#endif

namespace
{
	auto home_config()
	{
		return fmt::dir::join(env::usr::config_home, env::opt::identity);
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
			if (empty(env::opt::application))
			{
				env::opt::application = env::opt::program;
			}
			return env::opt::application;
		}

	} IDENTITY;

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
				auto const file = dirs.back();
				assert(not empty(file));
				auto const first = file.find_first_not_of("./");
				auto const last = file.rfind(sys::ext::image);
				u = file.substr(first, last);
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
				s = home_config();
				if (env::dir::fail(s))
				{
					fmt::string_view_span dirs = env::usr::config_dirs;
					for (auto const d : dirs)
					{
						s = fmt::dir::join(d, env::opt::identity);
						if (not env::dir::fail(s))
						{
							return s;
						}
					}
					s = home_config();
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
				s = fmt::dir::join(env::usr::cache_home, env::opt::identity);
			}
			return s;
		}

	} CACHE;

	ini open()
	{
		ini keys;
		auto const path = fmt::join({ env::opt::config, ".ini" });
		std::fstream file(path);
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
		if constexpr (fmt::is_floating<V>)
		{
			return std::isnan(n) ? value : n;
		}
		else
		{
			const auto d = static_cast<double>(n);
			return std::isnan(d) ? value : n;
		}
	}

	template <typename K>
	bool convert(K key, bool value)
	{
		auto const check = { "0", "no", "off", "disable" };
		auto const u = env::opt::get(key);
		if (not empty(u))
		{
			for (fmt::string_view v : check)
			{
				// if u.starts_with(v)
				if (0 == u.compare(0, size(v), v))
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
	env::view const& identity = IDENTITY;
	env::view const& program = PROGRAM;
	env::view const& config = CONFIG;
	env::view const& cache = CACHE;

	void set(int argc, char** argv)
	{
		auto back = back_inserter(ARGUMENTS.list);
		copy(argv, argv + argc, back);
	}

	view get(view key)
	{
		fmt::string_view_span args = env::opt::arguments;
		for (auto const argn : args)
		{
			const auto e = fmt::entry(argn);
			if (e.first == key)
			{
				return e.second;
			}
		}
		
		auto value = sys::env::get(key);
		if (empty(value))
		{
			auto const entry = make_pair(key);
			auto const unlock = lock.read();
			value = registry().get(entry);
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

	view get(pair key)
	{
		auto const unlock = lock.read();
		return registry().get(key);
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

	bool clear(pair key)
	{
		auto const unlock = lock.write();
		return registry().clear(key);
	}

	bool clear(view key)
	{
		auto const entry = make_pair(key);
		return clear(entry);
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
			return registry().clear(key);
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

