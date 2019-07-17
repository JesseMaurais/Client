// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "socket.hpp"
#include "sys.hpp"
#include "net.hpp"
#include "sig.hpp"
#include "int.hpp"
#include <cassert>

namespace sys::file
{
	socket::socket(int fd)
	{
		this->fd = fd;
	}

	socket::socket(int family, int type, int proto)
	{
		#ifdef _WIN32
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
		auto const pfd = sys::net::descriptor(sys::file::socket::fd);
		auto eq = [pfd](auto const& p) { return p.fd == pfd; };
		fds.erase(remove_if(begin(fds), end(fds), eq), end(fds));
		set.disconnect(pfd);
	}
}

