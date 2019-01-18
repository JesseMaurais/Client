// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "sock.hpp"
#include "sys.hpp"
#include "net.hpp"
#include "sig.hpp"
#include "err.hpp"

namespace sys::file
{
	socket::socket(std::intptr_t s)
	{
		this->s = s;
	}

	socket::socket(int family, int type, int proto)
	{
		#if defined(__WIN32__)
		{
			constexpr WORD version = MAKEWORD(2, 2);
			static sys::socket::wsadata ws(version);
		}
		#endif

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

	socket::operator bool() const
	{
		return not sys::socket::fail(s);
	}

	socket socket::accept(address& name, size_t* length) const
	{
		sys::socket::size m;
		socket t = sys::socket::accept(s, &name.address, &m);
		if (not t)
		{
			sys::perror("accept");
		}
		else 
		if (length)
		{
			*length = m;
		}
		return t;
	}

	bool socket::connect(address const& name, size_t length) const
	{
		if (sys::socket::connect(s, &name.address, length))
		{
			sys::socket::perror("connect");
			return false;
		}
		return true;
	}

	bool socket::bind(address const& name, size_t length) const
	{
		if (sys::socket::bind(s, &name.address, length))
		{
			sys::socket::perror("bind");
			return false;
		}
		return true;
	}

	bool socket::listen(int backlog) const
	{
		if (sys::socket::listen(s, backlog))
		{
			sys::socket::perror("listen");
			return false;
		}
		return true;
	}

	bool socket::shutdown(int how) const
	{
		if (sys::socket::shutdown(s, how))
		{
			sys::socket::perror("shutdown");
			return false;
		}
		return true;
	}

	ssize_t socket::send(const void *data, size_t size, int flags) const
	{
		auto p = static_cast<sys::socket::const_pointer>(data);
		ssize_t const n = sys::socket::send(s, p, size, flags);
		if (n < 0)
		{
			sys::socket::perror("send");
		}
		return n;
	}

	ssize_t socket::send(const void *data, size_t size, int flags, address const& name, size_t length) const
	{
		auto p = static_cast<sys::socket::const_pointer>(data);
		ssize_t const n = sys::socket::sendto(s, p, size, flags, &name.address, length);
		if (n < 0)
		{
			sys::socket::perror("sendto");
		}
		return n;
	}

	ssize_t socket::receive(void *data, size_t size, int flags) const
	{
		auto p = static_cast<sys::socket::pointer>(data);
		ssize_t const n = sys::socket::recv(s, p, size, flags);
		if (n < 0)
		{
			sys::socket::perror("recv");
		}
		return n;
	}

	ssize_t socket::receive(void *data, size_t size, int flags, address& name, size_t& length) const
	{
		sys::socket::size m = length;
		auto p = static_cast<sys::socket::pointer>(data);
		ssize_t const n = sys::socket::recvfrom(s, p, size, flags, &name.address, &m);
		if (n < 0)
		{
			sys::socket::perror("recvfrom");
		}
		else length = m;
		return n;
	}
}

namespace sig
{
	static subject<sys::socket::descriptor, short> set;
	static std::vector<sys::socket::pollfd> fds;

	int socket::poll(int timeout)
	{
		int const n = sys::socket::poll(fds.data(), fds.size(), timeout);
		if (n < 0)
		{
			sys::socket::perror("poll");
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
		sys::socket::pollfd p;
		p.fd = sys::socket::descriptor(sys::file::socket::s);
		p.events = events;

		set.connect(p.fd, [this](short events) { notify(events); });
		fds.push_back(p);
	}

	socket::~socket()
	{
		auto const fd = sys::socket::descriptor(sys::file::socket::s);
		stl::erase_if(fds, [fd](auto const& p) { return p.fd == fd; });
		verify(1 == set.disconnect(fd));
	}
}

