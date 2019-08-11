#include "opt.hpp"
#include "usr.hpp"
#include "dir.hpp"
#include "ini.hpp"
#include "fmt.hpp"
#include "err.hpp"
#include "sys.hpp"
#include <algorithm>
#include <iterator>
#include <fstream>

namespace
{
	auto home_config()
	{
		return fmt::dir::join(env::usr::config_home, env::opt::identity);
	}

	struct : env::list
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

	constexpr auto def = "Defaults";
}

namespace env::opt
{
	env::list const& arguments = ARGUMENTS;
	env::view const& identity = IDENTITY;
	env::view const& program = PROGRAM;
	env::view const& config = CONFIG;
	env::view const& cache = CACHE;
	fmt::string_view application;

	void set(int argc, char** argv)
	{
		auto it = back_inserter(ARGUMENTS.list);
		copy(argv, argv + argc, it);
	}

	view get(view key)
	{
		fmt::string_view_span args = env::opt::arguments;
		for (auto const ent : args)
		{
			const auto e = fmt::to_pair(ent);
			if (e.first == key)
			{
				return e.second;
			}
		}
		
		auto value = sys::env::get(key);
		if (empty(value))
		{
			ini::pair e { def, key };
			value = registry().get(e);
		}
		return value;
	}

	bool set(view key, view value)
	{
		ini::pair e { def, key };
		return set(e, value);
	}

	view get(pair key)
	{
		return registry().get(key);
	}

	bool set(pair key, view value)
	{
		return registry().set(key, value);
	}

	istream & get(istream & in)
	{
		return in >> registry();
	}

	ostream & put(ostream & out)
	{
		return out << registry();
	}
}

