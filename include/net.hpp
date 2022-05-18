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
	typedef const char *const_pointer;
	typedef ::SOCKET fd;
	typedef int size;
	typedef ::WSAPOLLFD pollfd;
	typedef ULONG nfds_t;
	typedef ::fd_set fd_set;
	typedef TIMEVAL timeval;

	union address
	{
		::SOCKADDR_STORAGE_LH pad;
		::SOCKADDR address;
		::SOCKADDR_IN in;
	};

	enum { rd = SD_RECEIVE, wr = SD_SEND, rw = SD_BOTH };

	constexpr auto invalid = INVALID_SOCKET;

	inline bool fail(fd value)
	{
		return invalid == value;
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
	typedef int fd;
	typedef ::socklen_t size;
	typedef ::pollfd pollfd;
	typedef ::nfds_t nfds_t;
	typedef ::fd_set fd_set;
	typedef ::timeval timeval;

	union address
	{
		::sockaddr_storage pad;
		::sockaddr address;
		::sockaddr_in in;
		::sockaddr_in6 in6;
		::sockaddr_un un;
	};

	enum { rd = SHUT_RD, wr = SHUT_WR, rw = SHUT_RDWR };

	constexpr auto invalid = -1;

	inline bool fail(fd value)
	{
		return invalid == value;
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
}

#endif // system
#endif // file
