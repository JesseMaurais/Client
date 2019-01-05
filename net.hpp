#ifndef net_hpp
#define net_hpp

#include "sys.hpp"

//
// POSIX
//

#if defined(__POSIX__) || defined(__UNIX__) || defined(__LINUX__)

#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <poll.h>

namespace sys::socket
{
	typedef void *pointer;
	typedef int descriptor;
	typedef ::socklen_t size;

	enum { in = SHUT_RD, out = SHUT_WR, both = SHUT_RDWR };

	constexpr descriptor invalid = -1;
	constexpr bool fail(descriptor fd) { return invalid == fd; }
	inline auto perror(char const *prefix) { return std::perror(prefix); }

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

} // sys::socket

//
// WIN32
//

#elif defined(__WIN32__) || defined(__OS2__)

#include <winsock2.h>

#define FD_ISSET(s, set) _WSAFDIsSet(s, set)

namespace sys::socket
{
	typedef char *pointer;
	typedef ::SOCKET descriptor;
	typedef int size;

	enum { in = SD_RECEIVE, out = SD_SEND, both = SD_BOTH };

	union address
	{
		struct ::sockaddr address;
	};

	constexpr descriptor invalid = ::INVALID_SOCKET;
	constexpr bool fail(descriptor h) { return invalid == h; }	
	inline auto perror(char const *prefix) { return sys::winerr(prefix); }	

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

} // sys::socket

#else
#error Cannot find system socket header
#endif

//
// Common
//

static_assert(sys::socket::fail(sys::socket::invalid));

namespace sys::socket
{
	union address
	{
		struct ::sockaddr address;
		struct ::sockaddr_in in;
	};
}

#endif // file

