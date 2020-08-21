// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "shell.hpp"
#include "desktop.hpp"
#include "dir.hpp"
#include "ps.hpp"
#include "err.hpp"
#include <exception>

namespace env
{
	struct shell & command = dialog;

	shell::line shell::get(in put, char end, int count)
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
		return { first, second, &cache };
	}

	shell::line shell::run(span arguments)
	{
		fmt::ipstream sub(arguments);
		auto const line = get(sub);
		status = sub.wait();
		return line;
	}

	shell::line shell::run(init arguments)
	{
		view::vector s(arguments);
		return run(s);
	}

	shell::line shell::list(view name)
	{
		return run
		(
		#ifdef _WIN32
			{ "dir", "/b", name }
		#else
			{ "ls", name }
		#endif
		);
	}

	shell::line shell::copy(view path)
	{
		return run
		(
		#ifdef _WIN32
			{ "type", path }
		#else
			{ "cat", path }
		#endif
		);
	}

	shell::line shell::find(view pattern, view directory)
	{
		#ifdef _WIN32
		{
			return list(fmt::dir::join(directory, pattern));
		}
		#else
		{
			return run({ "find", directory, "-type", "f", "-name", pattern });
		}
		#endif
	}

	shell::line shell::which(view name)
	{
		return run
		(
		#ifdef _WIN32
			{ "where", name }
		#else
			{ "which", "-a", name }
		#endif
		);
	}

	shell::line shell::open(view path)
	{
		#ifdef _WIN32
		{
			return run({ "start", "/d", path });
		}
		#else
		{
			string::view::pair const test [] =
			{ 
				{ "xfce", "exo-open" },
				{ "gnome", "gnome-open" },
				{ "kde", "kde-open" },
				{ "", "xdg-open" },
			};

			for (auto [session, program] : test)
			{
				if (empty(session) or desktop::current(session))
				{
					if (auto path = which(program); not path.empty())
					{
						continue;
					}

					return run({ program, path });
				}
			}
			return { 0, 0, &cache };
		}
		#endif
	}
}

#ifdef test
test(shell)
{
	struct env::shell sh;
	auto const list = sh.list(env::pwd());
	assert(not empty(list));
	auto const copy = sh.copy(__FILE__);
	assert(not empty(copy));
	// Copy range starts at 0, file numbering at 1
	assert(copy[__LINE__-1].find("Recursive find me text") != fmt::npos);
}
#endif

