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

	int execute(int fd[3], env::file::command line)
	{
		strings list;
		auto str = repack(line, list);
		return sys::execute(fd, list.size() - 1, list.data());
	}
}

namespace env::file
{
	console start(size_t argc, char const **argv) noexcept
	{
		console com;
		com.first = sys::execute(com.second, argc, argv);
		return com;
	}

	console start(command line) noexcept
	{
		console com;
		com.first = ::execute(com.second, line);
		return com;
	}

	void close(console com)
	{
		[[maybe_unused]] process const off(com);
	}

	bool stop(console com)
	{
		return sys::kill(com.first);
	}

	int wait(console com)
	{
		return sys::wait(com.first);
	}

	int dup(console com)
	{
		int line = invalid;
		int const err = com.second[2];
		if (not fail(err))
		{
			auto fd = descriptor(err);
		
			std::string s;
			io::rstream in(fd);
			for (line = 0; std::getline(in, s); ++line)
			{
				std::cerr << s;
			}
			fd.set();
		}
		return line;
	}
}

