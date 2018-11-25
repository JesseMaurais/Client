#ifndef sys_hpp
#define sys_hpp

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

//
// POSIX
//

#if __has_include(<unistd.h>)
#include <unistd.h>

namespace sys
{

// Portable Operating System Interface X
constexpr long POSIX_VERSION =
#if defined(_POSIX_VERSION)
	_POSIX_VERSION
#else
	0L
#endif
	;

// Single UNIX Specification
constexpr long XOPEN_VERSION =
#if defined(_XOPEN_VERSION)
	_XOPEN_VERSION
#else
	0L
#endif
	;

constexpr bool XSI = XOPEN_VERSION > 0;
constexpr bool POSIX = XSI or POSIX_VERSION > 0;
constexpr bool WINRT = false;

// Linux does not define/require these
#ifndef O_BINARY
#define O_BINARY 0L
#endif
#ifndef O_TEXT
#define O_TEXT 0L
#endif

constexpr auto dup2 = ::dup2;
constexpr auto open = ::open;
using ssize_t = ::ssize_t;
constexpr auto write = ::write;
constexpr auto read = ::read;
constexpr auto close = ::close;
using pid_t = ::pid_t;
constexpr auto fork = ::fork;
constexpr auto popen = ::popen;
constexpr auto pclose = ::pclose;
constexpr auto pipe = ::pipe;
constexpr auto execv = ::execv;

} // namespace sys

#else

//
// MSVCRT
//

#if __has_include(<io.h>) && __has_include(<process.h>)
#include <io.h>
#include <process.h>

namespace sys
{

constexpr bool XSI = false;
constexpr bool POSIX = false;

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#define O_APPEND _O_APPEND
#define O_BINARY _O_BINARY
#define O_CREAT  _O_CREAT
#define O_RDONLY _O_RDONLY
#define O_RDWR   _O_RDWR
#define O_TEXT   _O_TEXT
#define O_TRUNC  _O_TRUNC
#define O_WRONLY _O_WRONLY

constexpr auto dup2 = ::_dup2;
constexpr auto open = ::_open;
using ssize_t = int;
constexpr auto write = ::_write;
constexpr auto read = ::_read;
constexpr auto close = ::_close;

using pid_t = int;
// WIN32 does not have a fork
constexpr int _nosys = [] { ::_set_errno(ENOSYS); return -1; };
constexpr auto fork = [] { return _nosys() };

// UWP does not support the console
#ifndef _WINRT_DLL
constexpr bool WINRT = false;
constexpr auto popen = ::_popen;
constexpr auto pclose = ::_pclose;
constexpr auto pipe = [](int fd[2]) { return ::_pipe(fd, BUFSIZ, 0); };
constexpr auto execv = ::_execv;
#else
constexpr bool WINRT = true;
constexpr auto popen = [](char const *path, int mode) { return _nosys(); };
constexpr auto pclose = [](int fd) { return _nosys(); };
constexpr auto pipe = [](int fd[2]) { return _nosys(); };
constexpr auto execv = [](const char *cmd, const char *const argv[]) { return _nosys(); }
#endif // _WINRT_DLL

} // namespace sys

#else

//
// Unknown
//

#error Cannot find system headers
#endif // io.h
#endif // unistd.h
#endif // file