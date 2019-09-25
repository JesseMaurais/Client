// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "ipc.hpp"
#include "ips.hpp"
#include "sys.hpp"
#include "fmt.hpp"
#include "dig.hpp"
#include "opt.hpp"
#include <vector>

namespace
{
	using strings = std::vector<char const*>;

	auto repack(env::command line, strings &list)
	{
		fmt::string const z(1, '\0');
		auto s = fmt::join(line, z);
		for (auto u : fmt::split(s, z))
		{
			auto ptr = u.data();
			list.push_back(ptr);
		}
		list.push_back(nullptr);
		return s;
	}

	int execute(int fd[3], env::command line)
	{
		strings list;
		auto str = repack(line, list);
		return sys::execute(fd, list.size() - 1, list.data());
	}
}

namespace sys::file
{
	process::process(size_t argc, char const **argv)
	{
		int fd[3];
		pid = sys::execute(fd, argc, argv);
		if (not sys::fail(pid))
		{
			set(pid, fd);
		}
	}

	process::process(env::command line)
	{
		int fd[3];
		pid = ::execute(fd, line);
		if (not sys::fail(pid))
		{
			set(pid, fd);
		}
	}

	bool process::kill()
	{
		return sys::kill(pid);
	}

	int process::wait()
	{
		return sys::wait(pid);
	}
}

