#include "desktop.hpp"
#include "fmt.hpp"
#include "usr.hpp"
#include "ips.hpp"
#include <map>

namespace env::desktop
{
	bool is(fmt::string_view name)
	{
		auto const current = fmt::to_lower(env::usr::current_desktop);
		auto const lower = fmt::to_lower(name);
		return current.find(lower) != fmt::npos;
	}

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

	open::open(fmt::string_view path)
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
			fmt::map<fmt::string_view> const test
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

	dialog::dialog(fmt::span_view args)
	{
		constexpr auto program = "zenity";

		if (empty(where(program)))
		{
			return;
		}

		fmt::vector<fmt::string::const_pointer> list;

		auto s = fmt::join(args);
		for (auto u : fmt::split(s))
		{
			list.push_back(data(u));
		}
		list.push_back(nullptr);

		if (fail(start(list.size() - 1, list.data())))
		{
			sys::err(here, program, s);
		}
	}
}

