// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "ipc.hpp"
#include "sys.hpp"
#include "fmt.hpp"
#include "dig.hpp"
#include "opt.hpp"
#include <vector>

namespace
{
	using strings = std::vector<char const*>;

	auto repack(sys::command line, strings &list)
	{
		fmt::string const term(1, '\0');
		auto str = fmt::join(line, term);
		auto args = fmt::split(str, term);
		for (auto u : args)
		{
			auto ptr = u.data();
			list.push_back(ptr);
		}
		list.push_back(nullptr);
		return str;
	}

	int start(int fd[3], sys::command line)
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

	process::process(command line)
	{
		int fd[3];
		pid = start(fd, line);
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

namespace sys::ipc
{
	file::process twin(command line)
	{
		auto const path = ::env::opt::arg(0);
		auto const s = fmt::to_string(path);
		auto const c = s.c_str();
		assert(not empty(path));

		strings list;
		list.push_back(c);
		auto str = repack(line, list);

		return sys::file::process(list.size() - 1, list.data());
	}
}

