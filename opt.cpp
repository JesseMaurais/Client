#include "opt.hpp"
#include "usr.hpp"
#include "dir.hpp"
#include "ini.hpp"
#include "fmt.hpp"
#include "err.hpp"
#include "sys.hpp"
#include <fstream>

namespace
{
	struct : env::view
	{
		operator fmt::string_view() const final
		{
			static fmt::string s;
			if (empty(s))
			{
				s = fmt::dir::join(env::usr::config_home, env::opt::application);
			}
			return s;
		}

	} CONFIGURATION;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			if (empty(env::opt::application_name))
			{
				fmt::string_view u = env::opt::program;
				env::opt::application_name = fmt::to_string(u);
			}
			return env::opt::application_name;
		}

	} APPLICATION;

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
				auto const part = fmt::path::split(path);
				assert(not empty(part));
				auto const file = part.back();
				assert(not empty(file));
				auto const first = file.find_first_not_of("./");
				auto const last = file.rfind(sys::ext::image);
				u = file.substr(first, last);
			}
			return u;
		}

	} PROGRAM;

	struct : env::list
	{
		fmt::string_view_vector lines;

		operator fmt::string_view_span() const final
		{
			return lines;
		}

	} ARGUMENTS;
}

namespace env::opt
{
	fmt::string application_name;
	env::view const& configuration = CONFIGURATION;
	env::view const& application = APPLICATION;
	env::list const& arguments = ARGUMENTS;
	env::view const& program = PROGRAM;

	void init(int argc, char** argv)
	{
		assert(nullptr == argv[argc]);
		fmt::string_view_span args = ARGUMENTS;
		assert(empty(args));
		if (empty(args))
		{
			ARGUMENTS.lines.assign(argv, argv + argc);
		}
	}

	fmt::string_view get(fmt::string_view u)
	{
		auto pair = fmt::split(u, ":");
		if (empty(pair.second))
		{
			fmt::string_view_span t = env::opt::arguments;
			for (auto const v : t)
			{
				pair = fmt::to_pair(v);
				if (pair.first == u)
				{
					return pair.second;
				}
			}

			auto const v = sys::env::get(u);
			pair = fmt::to_pair(v);
			if (pair.first == u)
			{
				return pair.second;
			}
		}

		static ini::group t;
		if (empty(t))
		{
			
		}
	}
}

