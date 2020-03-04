#include "desktop.hpp"
#include "fmt.hpp"
#include "usr.hpp"
#include "ips.hpp"
#include <map>

namespace env::desktop
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
			fmt::map<fmt::string_view> const test
			{ 
				{ "gnome", "gnome-open" },
				{ "xfce", "exo-open" },
				{ "kde", "kde-open" },
				{ "", "xdg-open" },
			};

			auto const current = fmt::to_lower(env::usr::current_desktop);
			for (auto const& [desktop, program] : test)
			{
				if (empty(desktop) or desktop == current)
				{
					if (empty(where(program)))
					{
						continue;
					}
					else
					if (fail(sub.start({ program, path })))
					{
						sys::err(here, program, path);
					}
					else break;
				}
			}
		}
		#endif

		int fd[3], pid = sub.get(fd);
		sub.set();
		return process(pid, fd);
	}
}

