#ifndef net_hpp
#define net_hpp

#include "sys.hpp"

#ifdef _WIN32

#include <winsock2.h>
#include <afunix.h>

#pragma comment(lib, "Ws2_32.lib")

namespace sys::net
{
	typedef char *pointer;
	typedef char const *const_pointer;
	typedef ::SOCKET descriptor;
	typedef int size;
	typedef ::WSAPOLLFD pollfd;

	union address
	{
		::SOCKADDR_STORAGE_LH pad;
		::SOCKADDR address;
		::SOCKADDR_UN un;
		::SOCKADDR_IN in;
	};

	enum { in = SD_RECEIVE, out = SD_SEND, both = SD_BOTH };

	constexpr bool fail(descriptor h) { return INVALID_SOCKET == h; }
	inline void perror(char const *prefix) { ::_set_errno(::sys::winerr(prefix)); }

	constexpr auto close = ::closesocket;
	constexpr auto accept = ::accept;
	constexpr auto bind = ::bind;
	constexpr auto connect = ::connect;
	constexpr auto getsockname = ::getsockname;
	constexpr auto getsockopt = ::getsockopt;
	constexpr auto listen = ::listen;
	constexpr auto poll = ::WSAPoll;
	constexpr auto select = ::select;
	constexpr auto setsockopt = ::setsockopt;
	constexpr auto shutdown = ::shutdown;
	constexpr auto socket = ::socket;
	constexpr auto send = ::send;
	constexpr auto sendto = ::sendto;
	constexpr auto recv = ::recv;
	constexpr auto recvfrom = ::recvfrom;

	namespace win
	{
		struct data : ::WSADATA
		{
			data(WORD version)
			{
				if (error = ::WSAStartup(version, this); error)
				{
					::sys::winerr("WSAStartup");
				}
			}

			~data()
			{
				if (not error and ::WSACleanup())
				{
					::sys::winerr("WSACleanup");
				}
			}

			operator bool() const
			{
				return not error;
			}

			int error;
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

	union address
	{
		::sockaddr_storage pad;
		::sockaddr address;
		::sockaddr_un un;
		::sockaddr_in in;
		::sockaddr_in6 in6;
	};

	enum { in = SHUT_RD, out = SHUT_WR, both = SHUT_RDWR };

	constexpr bool fail(descriptor fd) { return ::sys::fail(fd); }
	inline void perror(char const *prefix) { std::perror(prefix); }

	constexpr auto close = ::sys::close;
	constexpr auto accept = ::accept;
	constexpr auto bind = ::bind;
	constexpr auto connect = ::connect;
	constexpr auto getsockname = ::getsockname;
	constexpr auto getsockopt = ::getsockopt;
	constexpr auto listen = ::listen;
	constexpr auto poll = ::poll;
	constexpr auto select = ::select;
	constexpr auto setsockopt = ::setsockopt;
	constexpr auto shutdown = ::shutdown;
	constexpr auto socket = ::socket;
	constexpr auto send = ::send;
	constexpr auto sendto = ::sendto;
	constexpr auto recv = ::recv;
	constexpr auto recvfrom = ::recvfrom;

} // sys::net

#endif

#endif // file

