#include "shell.hpp"
#include "err.hpp"
#include "desktop.hpp"

namespace env::shell
{
	span get(in::ref input)
	{
		static thread_local string::vector list;
		list.clear();
		string line;
		while (getline(input, line)) list.emplace_back(move(line));
		return line;
	}

	span dir(view name)
	{
		fmt::ipstream sub
		#ifdef _WIN32
			{ "dir", "/b", name };
		#else
			{ "ls" name };
		#endif
		return get(sub);
	}

	span where(view name)
	{
		fmt::ipstream sub
		#ifdef _WIN32
			{ "where", name };
		#else
			{ "which", "-a", name };
		#endif
		return get(sub);
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
				{ "xfce", "exo-open" },
				{ "gnome", "gnome-open" },
				{ "kde", "kde-open" },
				{ "", "xdg-open" },
			};

			for (auto [desktop, program] : test)
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

