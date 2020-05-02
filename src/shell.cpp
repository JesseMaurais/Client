#include "shell.hpp"
#include "desktop.hpp"
#include "err.hpp"
#include "dir.hpp"
#include "ps.hpp"
#include <except>

namespace env
{
	shell::line shell::get(in put, char end, int count)
	{
		// Result in span at back of cache
		const auto first = cache.size();
		try // process can crash
		{
			while (--count < 0 and std::getline(put, last, end))
			{
				cache.emplace_back(std::move(last));
			}
		}
		catch (std::exception const& err)
		{
			last = err.what();
		}
		// Go one past the cache's end
		const auto second = cache.size();
		return { cache, { first, second } };
	}

	shell::line shell::list(view name)
	{
		fmt::ipstream sub
		#ifdef _WIN32
			{ "dir", "/b", name };
		#else
			{ "ls" name };
		#endif
		return get(sub);
	}

	shell::line shell::copy(view name)
	{
		fmt::ipstream sub
		#ifdef _WIN32
			{ "type", name };
		#else
			{ "cat", name };
		#endif
		return get(sub);
	}

	shell::line shell::find(view pattern, view directory)
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

	shell::line shell::which(view name)
	{
		fmt::ipstream sub
		#ifdef _WIN32
			{ "where", name };
		#else
			{ "which", "-a", name };
		#endif
		return get(sub);
	}

	shell::line shell::open(view path)
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
			return { cache, { 0, 0 } };
		}
		#endif
	}
}

