// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "proc.hpp"
#include "sys.hpp"
#include "fmt.hpp"
#include "ips.hpp"
#include <vector>

namespace
{
	using strings = std::vector<char const*>;

	auto repack(env::file::command line, strings &list)
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
}

namespace env::file
{
	int process::start(size_t argc, char const **argv)
	{
		int fds[3];
		pid = sys::execute(fds, argc, argv);
		if (not fail(pid))
		{
			for (int n : { 0, 1, 2 })
			{
				fd[n].set(fds[n]);
			}
		}
		return pid;
	}

	int process::start(command line)
	{
		strings list;
		auto const s = repack(line, list);
		return start(list.size() - 1, list.data());
	}

	int process::quit()
	{
		return sys::kill(pid);
	}

	int process::wait()
	{
		return sys::wait(pid);
	}

	int process::dump()
	{
		auto line = invalid;
		
		std::string s;
		fmt::istream in(fd[2]);
		for (line = 0; std::getline(in, s); ++line)
		{
			std::cerr << s;
		}

		return line;
	}
}

