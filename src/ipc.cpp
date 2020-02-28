// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "ipc.hpp"
#include "ips.hpp"
#include "sys.hpp"
#include "fmt.hpp"
#include "dig.hpp"
#include "opt.hpp"
#include "file.hpp"
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
	ssize_t console::write(const void* buf, size_t sz) const
	{
		auto const n = sys::write(fd[0], buf, sz);
		if (fail(n))
		{
			sys::err(here, fd[0], sz);
		}
		return n;
	}

	ssize_t console::read(void* buf, size_t sz) const
	{
		auto const n = sys::read(fd[1], buf, sz);
		if (fail(n))
		{
			sys::err(here, fd[1], sz);
		}
		return n;
	}

	int console::start(size_t argc, char const **argv)
	{
		return pid = sys::execute(fd, argc, argv);
	}

	int console::start(command line)
	{
		strings list;
		auto const s = repack(line, list);
		return start(list.size() - 1, list.data());
	}

	int console::close(int pfd)
	{
		for (int n : { 0, 1, 2 })
		{
			if (fail(pfd) or n == pfd)
			{
				[[maybe_unused]] descriptor const closed(fd[n]);
			}
		}
		return 0;
	}

	int console::quit()
	{
		return sys::kill(pid);
	}

	int console::wait()
	{
		return sys::wait(pid);
	}

	int console::dump()
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

