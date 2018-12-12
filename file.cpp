#include "file.hpp"
#include "sys.hpp"
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
		if (mode & trunc)
		{
			flags |= O_TRUNC;
		}
		if constexpr (sys::LINUX)
		{
			flags |= O_DIRECT;
		}
		return flags;
	}
}

namespace sys::file
{
	size_t bufsiz = BUFSIZ;

	void descriptor::open(std::string const& path, openmode mode)
	{
		fd = sys::open(path.c_str(), convert(mode));
		if (-1 == fd)
		{
			sys::perror("open");
		}
	}

	ssize_t descriptor::write(const void* buffer, size_t size)
	{
		ssize_t const n = sys::write(fd, buffer, size);
		if (-1 == n)
		{
			sys::perror("write", fd, size);
		}
		return n;
	}

	ssize_t descriptor::read(void* buffer, size_t size)
	{
		ssize_t const n = sys::read(fd, buffer, size);
		if (-1 == n)
		{
			sys::perror("read", fd, size);
		}
		return n;
	}

	descriptor::~descriptor()
	{
		if (-1 != fd)
		{
			if (-1 == sys::close(fd))
			{
				sys::perror("close", fd);
			}
		}
	}

	pipe::pipe()
	{
		int pair[2];
		if (-1 == sys::pipe(pair))
		{
			sys::perror("pipe");
			return;
		}
		for (int i : { 0, 1 })
		{
			fd[i].set(pair[i]);
		}
	}

	void process::open(arguments args, openmode mode)
	{
		std::vector<char*> cmds;
		for (auto s : args) cmds.push_back(const_cast<char*>(s));
		cmds.push_back(nullptr);

		int fd[3];
		pid = sys::pexec(fd, cmds.data(), convert(mode));
		if (valid(pid))
		{
			
		}		
	}
}

