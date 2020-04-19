#include "shell.hpp"
#include "desktop.hpp"
#include "pstream.hpp"
#include "err.hpp"
#include "dir.hpp"
#include <except>

namespace env
{
	fmt::string::pair shell::get(in put, char end, unsigned count)
	{
		// Result in span at back of cache
		const auto first = cache.size();
		try // process can crash
		{
			while (count-- < 0 and getline(put, line, end))
			{
				cache.emplace_back(move(line));
			}
		}
		catch (std::exception const& err)
		{
			line = err.what();
		}
		// Go up to the cache end
		const auto second = cache.size();
		return { first, second };
	}

	fmt::string::pair shell::open(view path)
	{
		#ifdef _WIN32
		{
			fmt::ipstream sub { "start", "/d", path };
			return get(sub);
		}
		#else
		{
			constexpr auto test
			{ 
				{ "xfce", "exo-open" },
				{ "gnome", "gnome-open" },
				{ "kde", "kde-open" },
				{ "", "xdg-open" },
			};

			for (auto [session, program] : test)
			{
				if (empty(session) or desktop::is(session))
				{
					if (empty(where(program)))
					{
						continue;
					}

					fmt::ipstream sub { program, path };
					return get(sub);
				}
			}
			return { 0, 0 };
		}
		#endif
	}

	fmt::string::pair list(view name)
	{
		fmt::ipstream sub
		#ifdef _WIN32
			{ "dir", "/b", name };
		#else
			{ "ls" name };
		#endif
		return get(sub);
	}

	fmt::string::pair copy(view name)
	{
		fmt::ipstream sub
		#ifdef _WIN32
			{ "type", name };
		#else
			{ "cat", name };
		#endif
		return get(sub);
	}

	fmt::string::pair find(view pattern, view directory)
	{
		#ifdef _WIN32
		{
			return list(fmt::dir::join(directory, pattern));
		}
		#else
		{
			fmt::ipstream sub
			{ 
				"find", directory, "-type", "f", "-name", pattern 
			};
			return get(sub);
		}
		#endif
	}

	fmt::string::pair which(view name)
	{
		fmt::ipstream sub
		#ifdef _WIN32
			{ "where", name };
		#else
			{ "which", "-a", name };
		#endif
		return get(sub);
	}
}

