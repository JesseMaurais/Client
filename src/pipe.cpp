// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "pipe.hpp"
#include "sys.hpp"
#include "err.hpp"
#include "type.hpp"
#include <utility>

namespace env::file
{
	ssize_t descriptor::write(const void* buf, size_t sz) const
	{
		auto const n = sys::write(fd, buf, sz);
		if (fail(n))
		{
			sys::err(here, fd, sz);
		}
		return n;
	}

	ssize_t descriptor::read(void* buf, size_t sz) const
	{
		auto const n = sys::read(fd, buf, sz);
		if (fail(n))
		{
			sys::err(here, fd, sz);
		}
		return n;
	}

	bool descriptor::open(fmt::string::view path, mode am, permit pm)
	{
		if (not fmt::terminated(path))
		{
			auto const s = fmt::to_string(path);
			return open(s, am, pm);
		}
		auto const c = path.data();

		fd = sys::open(c, convert(am), convert(pm));
		if (fail(fd))
		{
			sys::err(here, path, am, pm);
			return failure;
		}
		return success;
	}

	bool descriptor::close()
	{
		if (fail(sys::close(fd)))
		{
			sys::err(here, fd);
			return failure;
		}
		fd = invalid;
		return success;
	}

	pipe::pipe()
	{
		int fd[2];
		if (fail(sys::pipe(fd)))
		{
			sys::err(here);
		}
		else set(fd);
	}

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

