// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "process.hpp"
#include "sys.hpp"
#include "fmt.hpp"
#include <vector>

namespace env::file
{
	bool process::start(fmt::list_view args)
	{
		fmt::vector_view list(args);
		return start(fmt::span_view(list));
	}

	bool process::start(fmt::span_view args)
	{
		fmt::vector<fmt::string::const_pointer> list;
		auto s = fmt::join(args);
		for (auto u : fmt::split(s))
		{
			list.push_back(data(u));
		}
		list.push_back(nullptr);
		return start(list.size() - 1, list.data());
	}

	bool process::start(size_t argc, char const **argv)
	{
		int fds[3];
		pid = sys::execute(fds, argc, argv);
		if (not fail(pid))
		{
			for (auto n : { 0, 1, 2 })
			{
				(void) fd[n].set(fds[n]);
			}
		}
		return fail(pid);
	}

	bool process::quit()
	{
		return fail(sys::kill(pid));
	}

	bool process::wait()
	{
		return fail(sys::wait(pid));
	}
}

