// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "file.hpp"
#include "sys.hpp"
#include "err.hpp"

namespace sys::file
{
	int access(mode bit)
	{
		int flags = 0;

		if (bit & ok)
		{
			flags |= F_OK;
		}
		if (bit & ex)
		{
			flags |= X_OK;
		}
		if (bit & rd)
		{
			flags |= R_OK;
		}
		if (bit & wr)
		{
			flags |= W_OK;
		}

		return flags;
	}

	int convert(mode bit)
	{
		int flags = 0;

		if (bit & rw)
		{
			flags |= O_RDWR;
		}
		else 
		if (bit & wr)
		{
			flags |= O_WRONLY;
		}
		else 
		if (bit & rd)
		{
			flags |= O_RDONLY;
		}

		if (bit & txt)
		{
			flags |= O_TEXT;
		}
		else
		if (bit & bin)
		{
			flags |= O_BINARY;
		}

		if (bit & app)
		{
			flags |= O_APPEND;
		}

		if (bit & sz)
		{
			flags |= O_TRUNC;
		}

		if (bit & xu)
		{
			flags |= O_EXCL;
		}

		if (bit & ok)
		{
			flags |= O_CREAT;
		}

		return flags;
	}

	int convert(permit bit)
	{
		int flags = 0;

		if (bit & owner_x)
		{
			flags |= S_IXUSR;
		}
		if (bit & owner_w)
		{
			flags |= S_IWUSR;
		}
		if (bit & owner_r)
		{
			flags |= S_IRUSR;
		}
		if (bit & group_x)
		{
			flags |= S_IXGRP;
		}
		if (bit & group_w)
		{
			flags |= S_IWGRP;
		}
		if (bit & group_r)
		{
			flags |= S_IRGRP;
		}
		if (bit & other_x)
		{
			flags |= S_IXOTH;
		}
		if (bit & other_w)
		{
			flags |= S_IWOTH;
		}
		if (bit & other_r)
		{
			flags |= S_IROTH;
		}

		return flags;
	}

	size_t bufsiz = BUFSIZ;

	void descriptor::open(char const* path, mode bit, permit id)
	{
		fd = sys::open(path, convert(bit), convert(id));
		if (fail(fd))
		{
			sys::err(here, "open", path, bit, id);
		}
	}

	ssize_t descriptor::write(const void* buffer, size_t size) const
	{
		ssize_t const n = sys::write(fd, buffer, size);
		if (fail(n))
		{
			sys::err(here, "write", fd, size);
		}
		return n;
	}

	ssize_t descriptor::read(void* buffer, size_t size) const
	{
		ssize_t const n = sys::read(fd, buffer, size);
		if (fail(n))
		{
			sys::err(here, "read", fd, size);
		}
		return n;
	}

	void descriptor::close()
	{
		if (fail(sys::close(fd)))
		{
			sys::err(here, "close", fd);
		}
		else fd = invalid;
	}

	pipe::pipe()
	{
		int fd[2];
		if (fail(sys::pipe(fd)))
		{
			sys::err(here, "pipe");
		}
		else set(fd);
	}

	void process::run(char const** argv)
	{
		int fd[3];
		pid = sys::run(fd, argv);
		if (not fail(pid))
		{
			set(fd);
		}
	}

	void process::quit()
	{
		sys::quit(pid);
	}

	void process::kill()
	{
		sys::kill(pid);
	}
	
	int process::wait()
	{
		return sys::wait(pid);
	}
}

