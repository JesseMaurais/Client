#ifndef net_hpp
#define net_hpp "Network Interface"

#include "sys.hpp"
#include "err.hpp"

#ifdef _WIN32

#include <winsock2.h>
#include "win.hpp"

#pragma comment(lib, "Ws2_32.lib")

namespace sys::net
{
	typedef char *pointer;
	typedef char const *const_pointer;
	typedef ::SOCKET descriptor;
	typedef int size;
	typedef ::WSAPOLLFD pollfd;
	typedef ULONG nfds_t;

	union address
	{
		::SOCKADDR_STORAGE_LH pad;
		::SOCKADDR address;
		::SOCKADDR_IN in;
	};

	enum { in = SD_RECEIVE, out = SD_SEND, both = SD_BOTH };

	inline bool fail(descriptor h)
	{
		return INVALID_SOCKET == h;
	}

	template <typename... Args>
	inline void err(Args... args)
	{
		if (sys::debug)
		{
			auto const no = WSAGetLastError();
			auto const s = sys::win::strerr(no);
			sys::warn(args..., s);
		}
	}

	inline auto close = ::closesocket;
	inline auto accept = ::accept;
	inline auto bind = ::bind;
	inline auto connect = ::connect;
	inline auto getsockname = ::getsockname;
	inline auto getsockopt = ::getsockopt;
	inline auto listen = ::listen;
	inline auto poll = ::WSAPoll;
	inline auto select = ::select;
	inline auto setsockopt = ::setsockopt;
	inline auto shutdown = ::shutdown;
	inline auto socket = ::socket;
	inline auto send = ::send;
	inline auto sendto = ::sendto;
	inline auto recv = ::recv;
	inline auto recvfrom = ::recvfrom;

	namespace win
	{
		struct data : ::WSADATA
		{
			data(WORD ver)
			{
				e = WSAStartup(ver, this);
				if (e)
				{
					err(here, "WSAStartup");
				}
			}

			~data()
			{
				if (0 == e and WSACleanup())
				{
					err(here, "WSACleanup");
				}
			}

			operator bool() const
			{
				return 0 == e;
			}

			int e;
		};
	}

} // sys::net

#else // _POSIX

#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <poll.h>

namespace sys::net
{
	typedef void *pointer;
	typedef void const *const_pointer;
	typedef int descriptor;
	typedef ::socklen_t size;
	typedef ::pollfd pollfd;
	typedef ::nfds_t nfds_t;

	union address
	{
		::sockaddr_storage pad;
		::sockaddr address;
		::sockaddr_un un;
		::sockaddr_in in;
		::sockaddr_in6 in6;
	};

	enum { in = SHUT_RD, out = SHUT_WR, both = SHUT_RDWR };

	inline bool fail(descriptor fd) 
	{
		return -1 == fd;
	}

	template <typename... Args>
	inline void err(Args... args)
	{
		sys::err(args...);
	}

	inline auto close = ::sys::close;
	inline auto accept = ::accept;
	inline auto bind = ::bind;
	inline auto connect = ::connect;
	inline auto getsockname = ::getsockname;
	inline auto getsockopt = ::getsockopt;
	inline auto listen = ::listen;
	inline auto poll = ::poll;
	inline auto select = ::select;
	inline auto setsockopt = ::setsockopt;
	inline auto shutdown = ::shutdown;
	inline auto socket = ::socket;
	inline auto send = ::send;
	inline auto sendto = ::sendto;
	inline auto recv = ::recv;
	inline auto recvfrom = ::recvfrom;

} // sys::net

#endif

#endif // file

