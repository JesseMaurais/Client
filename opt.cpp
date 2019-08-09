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

	struct mirror
	{
		static constexpr auto defaults = "Defaults";

		mirror()
		{
			path = fmt::join(env::opt::config, ".ini");
			std::ifstream in(path);
			fmt::string s, sector;
			while (ini::getline(in, s))
			{
				if (ini::section(s))
				{
					sector = s;
					continue;
				}

				assert(not empty(sector));
				auto const p = fmt::to_pair(s);
				put(sector, p);
			}
		}

		~mirror()
		{
			std::ofstream out(path);
			for (auto const& sector : cache)
			{
				out << "[" << sector.first << "]" << fmt::eol;
				for (auto const& entry : sector.second)
				{
					out << fmt::key(entry) << fmt::eol;
				}
				out << fmt::eol;
			}
		}

		fmt::string_view get(fmt::string_view_pair p)
		{
			auto const it = find(p.first);
			if (end() != it)
			{
				auto const key = it->second.find(p.second);
				if (it->end() != key)
				{
					return key->second;
				}
			}
			return "";
		}

		fmt::string_view get(fmt::string_view u)
		{
			return get({ defaults, u });
		}

		void put(fmt::string_view u, fmt::string_view_pair p)
		{
			const auto [std::ignore, unique] = cache[u].insert(p);
			if (not unique)
			{
				sys::warn(here, "overwrite", p.first, "with", p.second);
			}
		}

		void put(fmt::string_view_pair p)
		{
			put(defaults, p);
		}

	private:

		ini::group cache;
		fmt::string path;

	} registry;
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
			ARGUMENTS.lines.assign(argv, argv + argc);
		}
	}

	fmt::string_view get(fmt::string_view u)
	{
		fmt::string_view_span t = env::opt::arguments;
		for (auto const v : t)
		{
			const auto p = fmt::key(v);
			if (p.first == u)
			{
				return p.second;
			}
		}
		
		auto const v = sys::env::get(u);
		if (empty(v))
		{
			v = registry.get(u);
		}
		return v;
	}

	fmt::string_view get(fmt::string_view_pair p)
	{
		return registry.get(p);
	}

	void put(fmt::string_view u, fmt::string_view v)
	{
		registry.put({ u, v });
	}

	void put(fmt::string_view_pair p, fmt::string_view v)
	{
		registry.put(p.first, { p.second, v });
	}
}

