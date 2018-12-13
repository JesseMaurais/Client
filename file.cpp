#include "file.hpp"
#include "sys.hpp"
#include "os.hpp"
#include "err.hpp"

namespace
{
	using namespace sys::file;
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
		if (mode & trun)
		{
			flags |= O_TRUNC;
		}
		#if defined(__LINUX__)
		{
			flags |= O_DIRECT;
		}
		#endif
		return flags;
	}
}

namespace sys::file
{
	size_t bufsiz = BUFSIZ;

	void descriptor::open(std::string const& path, openmode mode)
	{
		fd = sys::open(path.c_str(), convert(mode), 0);
		if (fail(fd))
		{
			sys::perror("open");
		}
	}

	ssize_t descriptor::write(const void* buffer, size_t size)
	{
		ssize_t const n = sys::write(fd, buffer, size);
		if (fail(n))
		{
			sys::perror("write", fd, size);
		}
		return n;
	}

	ssize_t descriptor::read(void* buffer, size_t size)
	{
		ssize_t const n = sys::read(fd, buffer, size);
		if (fail(n))
		{
			sys::perror("read", fd, size);
		}
		return n;
	}

	bool descriptor::close()
	{
		if (not fail(fd))
		{
			if (fail(sys::close(fd)))
			{
				sys::perror("close", fd);
				return true;
			}
			fd = -1;
		}
		return false;
	}

	pipe::pipe()
	{
		int pair[2];
		if (fail(sys::pipe(pair)))
		{
			sys::perror("pipe");
		}
		else for (int i : { 0, 1 })
		{
			fd[i].set(pair[i]);
		}
	}

	void process::open(arguments args, openmode mode)
	{
		std::vector<char*> cmds;
		for (auto s : args) cmds.push_back(const_cast<char*>(s));
		cmds.push_back(nullptr);

		int desc[3];
		pid = sys::pexec(desc, cmds.data());
		if (not fail(pid))
		{
			fd[0].set(desc[0]);
			fd[1].set(desc[1]);
			error.set(desc[2]);
		}		
	}
}

