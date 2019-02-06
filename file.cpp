// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "file.hpp"
#include "sys.hpp"
#include "net.hpp"
#include "sig.hpp"
#include "err.hpp"
#include "int.hpp"
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

	void descriptor::open(char const* path, openmode mode)
	{
		fd = sys::open(path, convert(mode), 0);
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

	bool process::execute(char const** argv)
	{
		int fd[3];
		pid = sys::exec(fd, argv);
		bool const ok = not fail(pid);
		if (ok) set(fd);
		return ok;
	}

	void process::terminate()
	{
		if (not fail(pid))
		{
			sys::term(pid);
			for (int n : { 0, 1, 2 })
			{
				if (file[n]) file[n].close();
			}
			pid = -1;
		}
	}
	
	int process::wait()
	{
		return sys::wait(pid);
	}

	socket::socket(int fd)
	{
		this->fd = fd;
	}

	socket::socket(int family, int type, int proto)
	{
		#if defined(__WIN32__)
		{
			constexpr WORD version = MAKEWORD(2, 2);
			static sys::net::wsadata ws(version);
		}
		#endif

		fd = sys::net::socket(family, type, proto);
		if (sys::net::fail(fd))
		{
			sys::net::perror("socket");
		}
	}

	socket::~socket()
	{
		if (not sys::net::fail(fd) and sys::net::close(fd))
		{
			sys::net::perror("closesocket");
		}
	}

	socket::operator bool() const
	{
		return not sys::net::fail(fd);
	}

	socket socket::accept(address& name, size_t* length) const
	{
		sys::net::size n;
		socket s = sys::net::accept(fd, &name.address, &n);
		if (not s)
		{
			sys::net::perror("accept");
		}
		else 
		if (length)
		{
			*length = fmt::to_size(n);
		}
		return s;
	}

	bool socket::connect(address const& name, size_t length) const
	{
		if (sys::net::connect(fd, &name.address, length))
		{
			sys::net::perror("connect");
			return false;
		}
		return true;
	}

	bool socket::bind(address const& name, size_t length) const
	{
		if (sys::net::bind(fd, &name.address, length))
		{
			sys::net::perror("bind");
			return false;
		}
		return true;
	}

	bool socket::listen(int backlog) const
	{
		if (sys::net::listen(fd, backlog))
		{
			sys::net::perror("listen");
			return false;
		}
		return true;
	}

	bool socket::shutdown(int how) const
	{
		if (sys::net::shutdown(fd, how))
		{
			sys::net::perror("shutdown");
			return false;
		}
		return true;
	}

	ssize_t socket::write(const void *data, size_t size, int flags) const
	{
		auto ptr = static_cast<sys::net::const_pointer>(data);
		ssize_t const n = sys::net::send(fd, ptr, size, flags);
		if (n < 0)
		{
			sys::net::perror("send");
		}
		return n;
	}

	ssize_t socket::write(const void *data, size_t size, int flags, address const& name, size_t length) const
	{
		auto ptr = static_cast<sys::net::const_pointer>(data);
		ssize_t const n = sys::net::sendto(fd, ptr, size, flags, &name.address, length);
		if (n < 0)
		{
			sys::net::perror("sendto");
		}
		return n;
	}

	ssize_t socket::read(void *data, size_t size, int flags) const
	{
		auto ptr = static_cast<sys::net::pointer>(data);
		ssize_t const n = sys::net::recv(fd, ptr, size, flags);
		if (n < 0)
		{
			sys::net::perror("recv");
		}
		return n;
	}

	ssize_t socket::read(void *data, size_t size, int flags, address& name, size_t& length) const
	{
		sys::net::size m = length;
		auto ptr = static_cast<sys::net::pointer>(data);
		ssize_t const n = sys::net::recvfrom(fd, ptr, size, flags, &name.address, &m);
		if (n < 0)
		{
			sys::net::perror("recvfrom");
		}
		else length = fmt::to_size(m);
		return n;
	}
}

namespace sig
{
	static subject<sys::net::descriptor, short> set;
	static std::vector<sys::net::pollfd> fds;

	int socket::poll(int timeout)
	{
		int const n = sys::net::poll(fds.data(), fds.size(), timeout);
		if (n < 0)
		{
			sys::net::perror("poll");
		}
		else
		for (unsigned i = 0, j = 0, k = n; j < k; ++i)
		{
			assert(fds.size() > i);
			auto const& p = fds[i];

			if (p.revents)
			{
				set.send(p.fd, [&p](auto& it)
				{
					assert(p.fd == it.first);
					it.second(p.revents);
				});

				if (j < i)
				{
					std::swap(fds[i], fds[j]);
				}

				++j;
			}
		}
		return n;
	}

	socket::socket(int family, int type, int proto, short events)
	: sys::file::socket(family, type, proto)
	{
		sys::net::pollfd p;
		p.fd = sys::net::descriptor(sys::file::socket::fd);
		p.events = events;

		set.connect(p.fd, [this](short events) { notify(events); });
		fds.push_back(p);
	}

	socket::~socket()
	{
		auto const fd = sys::net::descriptor(sys::file::socket::fd);
		stl::erase_if(fds, [fd](auto const& p) { return p.fd == fd; });
		set.disconnect(fd);
	}
}

