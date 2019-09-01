// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "file.hpp"
#include "sys.hpp"
#include "err.hpp"
#include "fmt.hpp"

namespace sys::file
{
	int access(mode am)
	{
		int flags = 0;

		if (am & ok)
		{
			flags |= F_OK;
		}
		if (am & ex)
		{
			flags |= X_OK;
		}
		if (am & rd)
		{
			flags |= R_OK;
		}
		if (am & wr)
		{
			flags |= W_OK;
		}

		return flags;
	}

	int convert(mode am)
	{
		int flags = 0;

		if (am & rw)
		{
			flags |= O_RDWR;
		}
		else 
		if (am & wr)
		{
			flags |= O_WRONLY;
		}
		else 
		if (am & rd)
		{
			flags |= O_RDONLY;
		}

		if (am & txt)
		{
			flags |= O_TEXT;
		}
		else
		if (am & bin)
		{
			flags |= O_BINARY;
		}

		if (am & app)
		{
			flags |= O_APPEND;
		}

		if (am & sz)
		{
			flags |= O_TRUNC;
		}

		if (am & xu)
		{
			flags |= O_EXCL;
		}

		if (am & ok)
		{
			flags |= O_CREAT;
		}

		return flags;
	}

	int convert(permit pm)
	{
		int flags = 0;

		if (pm & owner_x)
		{
			flags |= S_IXUSR;
		}
		if (pm & owner_w)
		{
			flags |= S_IWUSR;
		}
		if (pm & owner_r)
		{
			flags |= S_IRUSR;
		}
		if (pm & group_x)
		{
			flags |= S_IXGRP;
		}
		if (pm & group_w)
		{
			flags |= S_IWGRP;
		}
		if (pm & group_r)
		{
			flags |= S_IRGRP;
		}
		if (pm & other_x)
		{
			flags |= S_IXOTH;
		}
		if (pm & other_w)
		{
			flags |= S_IWOTH;
		}
		if (pm & other_r)
		{
			flags |= S_IROTH;
		}

		return flags;
	}

	size_t const bufsiz = BUFSIZ;

	bool fail(string_view path, mode am)
	{
		if (not fmt::terminated(path))
		{
			auto const s = fmt::to_string(path);
			return fail(s, am);
		}

		return sys::fail(sys::access(data(path), access(am)));
	}

	bool descriptor::open(string_view path, mode am, permit pm)
	{
		if (not fmt::terminated(path))
		{
			auto const s = fmt::to_string(path);
			return open(s, am, pm);
		}

		fd = sys::open(data(path), convert(am), convert(pm));
		if (sys::fail(fd))
		{
			sys::err(here, path, am, pm);
			return failure;
		}
		return success;
	}

	ssize_t descriptor::write(const void* buf, size_t sz) const
	{
		auto const n = sys::write(fd, buf, sz);
		if (sys::fail(n))
		{
			sys::err(here, fd, sz);
		}
		return n;
	}

	ssize_t descriptor::read(void* buf, size_t sz) const
	{
		auto const n = sys::read(fd, buf, sz);
		if (sys::fail(n))
		{
			sys::err(here, fd, sz);
		}
		return n;
	}

	bool descriptor::close()
	{
		if (sys::fail(sys::close(fd)))
		{
			sys::err(here, fd);
			return failure;
		}
		else fd = invalid;
		return success;
	}

	pipe::pipe()
	{
		int fd[2];
		if (sys::fail(sys::pipe(fd)))
		{
			sys::err(here);
		}
		else set(fd);
	}

	bool process::execute(char const** argv)
	{
		int fd[3];
		pid = sys::execute(fd, argv);
		if (sys::fail(pid))
		{
			return failure;
		}
		else set(fd);
		return success;
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

