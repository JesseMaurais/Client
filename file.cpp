// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "file.hpp"
#include "sys.hpp"
#include "err.hpp"

namespace sys::file
{
	int convert(openmode mode)
	{
		int flags = 0;
		if (mode & in and mode & out)
		{
			flags |= O_RDWR;
		}
		else if (mode & out)
		{
			flags |= O_WRONLY;
		}
		else if (mode & in)
		{
			flags |= O_RDONLY;
		}
		if (mode & bin)
		{
			flags |= O_BINARY;
		}
		if (mode & app)
		{
			flags |= O_APPEND;
		}
		if (mode & sys::file::trunc)
		{
			flags |= O_TRUNC;
		}
		return flags;
	}

	int convert(mode bit)
	{
		int mask = 0;
		if (bit & mode::ok)
		{
			mask |= F_OK;
		}
		if (bit & mode::run)
		{
			mask |= X_OK;
		}
		if (bit & mode::read)
		{
			mask |= R_OK;
		}
		if (bit & mode::write)
		{
			mask |= W_OK;
		}
		return mask;
	}

	size_t bufsiz = BUFSIZ;

	void descriptor::open(char const* path, openmode mode)
	{
		fd = sys::open(path, convert(mode), 0);
		if (sys::fail(fd))
		{
			sys::perror("open");
		}
	}

	ssize_t descriptor::write(const void* buffer, size_t size) const
	{
		ssize_t const n = sys::write(fd, buffer, size);
		if (sys::fail(n))
		{
			sys::perror("write", fd, size);
		}
		return n;
	}

	ssize_t descriptor::read(void* buffer, size_t size) const
	{
		ssize_t const n = sys::read(fd, buffer, size);
		if (sys::fail(n))
		{
			sys::perror("read", fd, size);
		}
		return n;
	}

	void descriptor::close()
	{
		if (not sys::fail(fd))
		{
			if (sys::fail(sys::close(fd)))
			{
				sys::perror("close", fd);
			}
			else fd = -1;
		}
	}

	pipe::pipe()
	{
		int fd[2];
		if (sys::fail(sys::pipe(fd)))
		{
			sys::perror("pipe");
		}
		else set(fd);
	}

	bool process::run(char const** argv)
	{
		int fd[3];
		pid = sys::run(fd, argv);
		bool const ok = not sys::fail(pid);
		if (ok) set(fd);
		return ok;
	}

	void process::kill()
	{
		if (not sys::fail(pid))
		{
			sys::kill(pid);
			for (int n : { 0, 1, 2 })
			{
				if (file[n])
				{
					file[n].close();
				}
			}
			pid = -1;
		}
	}
	
	int process::wait()
	{
		return sys::wait(pid);
	}
}

