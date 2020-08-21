// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "process.hpp"
#include "type.hpp"
#include "sys.hpp"
#include <vector>

namespace env::file
{
	bool process::start(fmt::string::view::init args)
	{
		fmt::string::view::vector t(args);
		return start(fmt::string::view::span(t));
	}

	bool process::start(fmt::string::view::span args)
	{
		fmt::string::view const del("\0", 1);
		std::vector<char const *> list;
		auto s = fmt::join(args, del);
		for (auto u : fmt::split(s, del))
		{
			list.push_back(data(u));
		}

		auto const argc = list.size();
		list.push_back(nullptr);
		auto const argv = list.data();

		return start(argc, argv);
	}

	bool process::start(size_t argc, char const **argv)
	{
		int fds[3];
		pid = sys::exec(fds, argc, argv);
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

	int process::wait()
	{
		return sys::wait(pid);
	}
}

