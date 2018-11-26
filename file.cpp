#include "file.hpp"
#include "sys.hpp"
#include "err.hpp"
#include "os.hpp"

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
	void descriptor::open(std::string_view view, openmode mode)
	{
		std::string const path = sys::file::absolute(view).string();
		int const fd = sys::open(path.c_str(), convert(mode));
		if (-1 == fd)
		{
			sys::perror("open");
		}
	}

	descriptor::~descriptor()
	{
		if (-1 == fd)
		{
			return;
		}
		if (sys::close(fd))
		{
			sys::perror("close", fd);
		}
	}

	pipe::pipe()
	{
		int fds[2];
		if (sys::pipe(fds))
		{
			sys::perror("pipe");
			return;
		}
		this->fds[0] = fds[0];
		this->fds[1] = fds[1];
	}
}

