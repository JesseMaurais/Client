// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "pipe.hpp"
#include "sys.hpp"
#include "err.hpp"
#include "fmt.hpp"
#include <utility>
#ifdef _WIN32
#include "win/sync.hpp"
#else
#include "uni/pthread.hpp"
#endif

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

	bool descriptor::open(fmt::string_view path, mode am, permit pm)
	{
		if (not fmt::terminated(path))
		{
			auto const s = fmt::to_string(path);
			return open(s, am, pm);
		}

		auto const fd = sys::open(data(path), convert(am), convert(pm));
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
}

