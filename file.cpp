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
		if (bit & extant)
		{
			flags |= F_OK;
		}
		if (bit & run)
		{
			flags |= X_OK;
		}
		if (bit & read)
		{
			flags |= R_OK;
		}
		if (bit & write)
		{
			flags |= W_OK;
		}
		return flags;
	}

	int convert(mode bit)
	{
		int flags = 0;

		if (bit & read and bit & write)
		{
			flags |= O_RDWR;
		}
		else 
		if (bit & write)
		{
			flags |= O_WRONLY;
		}
		else 
		if (bit & read)
		{
			flags |= O_RDONLY;
		}

		if (bit & text)
		{
			flags |= O_TEXT;
		}
		else
		if (bit & binary)
		{
			flags |= O_BINARY;
		}

		if (bit & append)
		{
			flags |= O_APPEND;
		}

		if (bit & erase)
		{
			flags |= O_TRUNC;
		}

		if (bit & only)
		{
			flags |= O_EXCL;
		}

		if (~bit & extant)
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

	void descriptor::open(char const* path, mode mask)
	{
		fd = sys::open(path, convert(mask), 0);
		if (sys::fail(fd))
		{
			sys::perror("open", path, mask);
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

	void process::run(char const** argv)
	{
		int fd[3];
		pid = sys::run(fd, argv);
		if (not fail(pid))
		{
			set(fd);
		}
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

