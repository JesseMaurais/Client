#include "app.hpp"
#include "ips.hpp"
#include <map>

namespace env::app
{
	fmt::vector_string where(fmt::string_view name)
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

	process open(fmt::string_view path)
	{
		process sub;
		#ifdef _WIN32
		{
			if (fail(sub.start({ "start", "/d", path })))
			{
				sys::err(here, path);
			}
		}
		#else
		{
			fmt::map<fmt::string_view> const map
			{ 
				{ "gnome", "gnome-open" },
				{ "xfce", "exo-open" },
				{ "kde", "kde-open" },
				{ "", "xdg-open" },
			};

			auto const desktop = fmt::to_lower(env::usr::current_desktop);
			for (auto const pair : map)
			{
				if (pair.first.empty() or desktop == pair.first)
				{
					auto const tool = where(pair.second);
					if (empty(tool))
					{
						continue;
					}

					if (fail(sub.open({ pair.second, path })))
					{
						sys::err(here, pair.second, path);	
					}
					else break;
				}
			}
		}
		#endif
		return sub;
	}
}

