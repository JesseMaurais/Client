#include "file.hpp"
#include "sys.hpp"
#include "net.hpp"
#include "sig.hpp"
#include "err.hpp"
#include "dbg.hpp"
#include "alg.hpp"

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
		if (mode & sys::file::trunc)
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

	ssize_t descriptor::write(const void* buffer, size_t size) const
	{
		ssize_t const n = sys::write(fd, buffer, size);
		if (fail(n))
		{
			sys::perror("write", fd, size);
		}
		return n;
	}

	ssize_t descriptor::read(void* buffer, size_t size) const
	{
		ssize_t const n = sys::read(fd, buffer, size);
		if (fail(n))
		{
			sys::perror("read", fd, size);
		}
		return n;
	}

	void descriptor::close()
	{
		if (not fail(fd))
		{
			if (fail(sys::close(fd)))
			{
				sys::perror("close", fd);
			}
			else fd = -1;
		}
	}

	pipe::pipe()
	{
		int fd[2];
		if (fail(sys::pipe(fd)))
		{
			sys::perror("pipe");
		}
		else set(fd);
	}

	bool process::execute(arguments args)
	{
		std::vector<char*> cmds;
		for (auto s : args) cmds.push_back(const_cast<char*>(s));
		cmds.push_back(nullptr);

		int fd[3];
		pid = sys::pexec(fd, cmds.data());
		bool const ok = not fail(pid);
		if (ok) set(fd);
		return ok;
	}

	void process::terminate()
	{
		pid = sys::terminate(pid);
		for (int n : { 0, 1, 2 })
		{
			if (file[n]) file[n].close();
		}
	}

	socket::socket()
	{
		s = sys::socket::invalid;
	}

	socket::socket(std::intptr_t s)
	{
		this->s = s;
	}

	socket::socket(int family, int type, int proto)
	{
		s = sys::socket::socket(family, type, proto);
		if (sys::socket::fail(s))
		{
			sys::perror("socket");
		}
	}

	socket::~socket()
	{
		if (not sys::socket::fail(s) and sys::socket::close(s))
		{
			sys::perror("closesocket");
		}
	}

	socket::operator bool()
	{
		return not sys::socket::fail(s);
	}

	socket socket::accept(address& name, size_t* size)
	{
		sys::socket::size length;
		socket t = sys::socket::accept(s, &name.address, &length);
		if (not t)
		{
			sys::perror("accept");
		}
		else 
		if (size)
		{
			*size = length;
		}
		return t;
	}

	bool socket::connect(address const& name, size_t size)
	{
		if (not sys::socket::fail(s) and sys::socket::connect(s, &name.address, size))
		{
			sys::socket::perror("connect");
			return false;
		}
		return true;
	}

	bool socket::bind(address const& name, size_t size)
	{
		if (not sys::socket::fail(s) and sys::socket::bind(s, &name.address, size))
		{
			sys::socket::perror("bind");
			return false;
		}
		return true;
	}

	bool socket::listen(int backlog)
	{
		if (not sys::socket::fail(s) and sys::socket::listen(s, backlog))
		{
			sys::socket::perror("listen");
			return false;
		}
		return true;
	}
}

namespace sig
{
	subject<sys::socket::descriptor, int> set;
	std::vector<sys::socket::pollfd> fds;

	int socket::poll(int timeout)
	{
		int const count = sys::socket::poll(fds.data(), fds.size(), timeout);
		if (count < 0)
		{
			sys::socket::perror("poll");
		}
		for (int i = 0, j = 0; j < count; ++i)
		{
			assert(fds.size() > i);
			auto const& p = fds[i];

			if (p.revents)
			{
				++j;

				set.send(p.fd, [&p](auto& it)
				{
					assert(p.fd == it.first);
					it.second(p.revents);
				});
			}
		}
		return count;
	}

	socket::socket(int family, int type, int proto, int events)
	: sys::file::socket(family, type, proto)
	{
		sys::socket::pollfd p;
		p.fd = sys::socket::descriptor(sys::file::socket::s);
		p.events = events;

		set.connect(p.fd, [this](int event) { notify(event); });
		fds.push_back(p);
	}

	socket::~socket()
	{
		auto const fd = sys::socket::descriptor(sys::file::socket::s);
		stl::erase_if(fds, [fd](auto const& p) { return p.fd == fd; });
		verify(1 == set.disconnect(fd));
	}
}

