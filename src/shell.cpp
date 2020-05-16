#include "shell.hpp"
#include "desktop.hpp"
#include "err.hpp"
#include "dir.hpp"
#include "ps.hpp"
#include <except>

namespace env
{
	shell::lines shell::get(in put, char end, int count)
	{
		// Result in span at cache end
		const auto first = cache.size();
		auto second = first;
		try // process can crash
		{
			while (--count and std::getline(put, last, end))
			{
				cache.emplace_back(std::move(last));
			}
			// One past the end
			second = cache.size();
		}
		// Put exception message into line
		catch (std::exception const& error)
		{
			last = error.what();
		}
		return { { first, second }, cache };
	}

	shell::lines shell::list(view name)
	{
		fmt::ipstream sub
		#ifdef _WIN32
			{ "dir", "/b", name };
		#else
			{ "ls" name };
		#endif
		return get(sub);
	}

	shell::lines shell::copy(view path, int count)
	{
		fmt::ipstream sub
		#ifdef _WIN32
			{ "type", path };
		#else
			{ "cat", path };
		#endif
		return get(sub);
	}

	shell::lines shell::find(view pattern, view directory)
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

	shell::lines shell::which(view name)
	{
		fmt::ipstream sub
		#ifdef _WIN32
			{ "where", name };
		#else
			{ "which", "-a", name };
		#endif
		return get(sub);
	}

	shell::lines shell::open(view path)
	{
		#ifdef _WIN32
		{
			fmt::ipstream sub 
				{ "start", "/d", path };
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

					fmt::ipstream sub 
						{ program, path };
					return get(sub);
				}
			}
			return { { 0, 0 }, cache };
		}
		#endif
	}
}

#ifndef NDEBUG
void test::run<test::unit::shell>() noexcept
{
	env::shell cmd;

	auto const list = cmd.list(env::pwd);
	assert(not empty(list));
	auto const copy = cmd.copy(__FILE__);
	assert(copy.cache.at(__LINE__).find("assert") != fmt::npos);
}
#endif

