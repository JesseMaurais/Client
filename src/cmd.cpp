#include "cmd.hpp"
#include "err.hpp"
#include "pid.hpp"

namespace env::cmd
{
	fmt::string::vector where(fmt::string::view name)
	{
		#ifdef _WIN32
		fmt::ipstream sub { "where", name };
		#else
		fmt::ipstream sub { "which", "-a", name };
		#endif

		fmt::string line;
		fmt::vector_string list;
		while (std::getline(sub, line))
		{
			list.emplace_back(move(line));
		}
		return list;
	}

	open::open(fmt::string::view path)
	{
		#ifdef _WIN32
		{
			if (fail(start({ "start", "/d", path })))
			{
				sys::err(here, path);
			}
		}
		#else
		{
			fmt::string::view::map const test
			{ 
				{ "gnome", "gnome-open" },
				{ "xfce", "exo-open" },
				{ "kde", "kde-open" },
				{ "", "xdg-open" },
			};

			for (auto const& [desktop, program] : test)
			{
				if (empty(desktop) or is(desktop))
				{
					if (empty(where(program)))
					{
						continue;
					}
					else
					if (fail(start({ program, path })))
					{
						sys::err(here, program, path);
					}
					else break;
				}
			}
		}
		#endif
	}
}

