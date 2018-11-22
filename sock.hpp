#ifndef sock_hpp
#define sock_hpp

#include <sys.hpp>

#if __has_include(<sys/socket.h>)
#include <sys/socket.h>
#if __has_include(<sys/select.h>)
#include <sys/select.h>
#endif
#if __has_include(<poll.h>)
#include <poll.h>
#endif

#define SD_RECEIVE SHUT_RD
#define SD_SEND    SHUT_WR
#define SD_BOTH    SHUT_RDWR
#define SOCKET_ERRNO std::errno
using SOCKET_PTR = void*;
using SOCKET = int;
constexpr SOCKET INVALID_SOCKET = -1;
constexpr int SOCKET_ERROR = -1;
constexpr auto closesocket = sys::close;

#else

#if __has_include(<winsock2.h>)
#include <winsock2.h>

#define SHUT_RD   SD_RECEIVE
#define SHUT_WR   SD_SEND
#define SHUT_RDWR SD_BOTH
#define SOCKET_ERRNO WSAGetLastError()
using SOCKET_PTR = char*;
using socklen_t = int;
constexpr auto poll = ::WSAPoll;

#else
#error Cannot find system socket header
#endif // winsock2.h
#endif // poll.h

namespace sys::sock
{
	using addr = ::sockaddr;
	constexpr auto accept = ::accept;
	constexpr auto bind = ::bind;
	constexpr auto close = ::closesocket;
	constexpr auto connect = ::connect;
	constexpr auto getsockname = ::getsockname;
	constexpr auto getsockpt = ::getsockopt;
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
	constexpr auto htonl = ::htonl;
	constexpr auto ntohl = ::ntohl;
	constexpr auto htons = ::htons;
	constexpr auto ntohs = ::ntohs;
}

#endif // file

