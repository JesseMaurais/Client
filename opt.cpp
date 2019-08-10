#include "opt.hpp"
#include "usr.hpp"
#include "dir.hpp"
#include "ini.hpp"
#include "fmt.hpp"
#include "err.hpp"
#include "sys.hpp"
#include <fstream>
#include <tuple>

namespace
{
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
				s = fmt::dir::join(env::usr::config_home, env::opt::identity);
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

	ini::keys open()
	{
		auto const path = fmt::join({ env::opt::config, ".ini" });
		std::fstream file(path);
		ini::keys keys(file);
		return keys;
	}

	ini::keys & registry()
	{
		static auto keys = open();
		return keys;
	}

	constexpr auto defaults = "Defaults";
}

namespace env::opt
{
	env::list const& arguments = ARGUMENTS;
	env::view const& identity = IDENTITY;
	env::view const& program = PROGRAM;
	env::view const& config = CONFIG;
	env::view const& cache = CACHE;
	fmt::string_view application;

	void init(int argc, char** argv)
	{
		fmt::string_view_span args = ARGUMENTS;
		assert(empty(args));
		if (empty(args))
		{
			ARGUMENTS.list.assign(argv, argv + argc);
		}
	}

	fmt::string_view get(fmt::string_view u)
	{
		fmt::string_view_span t = env::opt::arguments;
		for (auto const v : t)
		{
			const auto p = fmt::to_pair(v);
			if (p.first == u)
			{
				return p.second;
			}
		}
		
		auto v = sys::env::get(u);
		if (empty(v))
		{
			ini::entry e { defaults, u };
			v = registry().get(e);
		}
		return v;
	}

	void put(fmt::string_view u, fmt::string_view v)
	{
		ini::entry e { defaults, u };
		put(e, v);
	}

	fmt::string_view get(fmt::string_view_pair p)
	{
		return registry().get(p);
	}

	void put(fmt::string_view_pair p, fmt::string_view v)
	{
		registry().put(p, v);
	}
}

