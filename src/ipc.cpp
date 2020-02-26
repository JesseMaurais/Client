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
		return pid = sys::execute(fd, argc, argv);
	}

	int process::start(fmt::span_view args)
	{
		strings list;
		auto const s = repack(args, list);
		return start(list.size() - 1, list.data());
	}

	int close(int pfd)
	{
		for (int n : { 0, 1, 2 })
		{
			if (fail(pfd) or n == fd)
			{
				[[maybe_unused]] descriptor(fd[n]);
			}
		}
	}

	bool process::stop()
	{
		return sys::kill(pid);
	}

	int process::wait()
	{
		return sys::wait(pid);
	}

	int process::dup()
	{
		auto line = invalid;
		if (not fail(fd[2]))
		{
			auto err = descriptor(fd[2]);
		
			std::string s;
			io::rstream in(err);
			for (line = 0; std::getline(in, s); ++line)
			{
				std::cerr << s;
			}
			err.set();
		}
		return line;
	}
}

