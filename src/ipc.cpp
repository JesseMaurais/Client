// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "ipc.hpp"
#include "sys.hpp"
#include "fmt.hpp"
#include "dig.hpp"
#include <vector>

namespace sys::file
{
	process::process(command cmd)
	{
		std::string const term(1, '\0');
		std::vector<char const*> list;
		auto line = fmt::join(cmd, term);
		auto args = fmt::split(line, term);
		for (auto str : args)
		{
			auto ptr = str.data();
			list.push_back(ptr);
		}
		list.push_back(nullptr);

		int argc = fmt::to_int(list.size());
		char const **argv = list.data();

		int fd[3];
		pid = sys::execute(fd, argc, argv);
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

