#ifndef sys_hpp
#define sys_hpp

#include "os.hpp"
#include "cc.hpp"

//
// Common
//

namespace sys
{

// Single UNIX Specification

constexpr bool xopen = 
#if defined(_XOPEN_SOURCE)
# undef __XOPEN__
# define __XOPEN__ 1
	true
#else
	false
#endif
	;

// Portable Operating System Interface

constexpr bool posix = 
#if defined(_POSIX_SOURCE) || defined(_POSIX_C_SOURCE) || defined(_XOPEN_SOURCE)
# undef __POSIX__
# define __POSIX__ 1
	true
#else
	false
#endif
	;

// Windows Runtime / Universal Windows Platform

constexpr bool winrt =
#if defined(_WINRTDLL)
# undef __WINRT__
# define __WINRT__ 1
	true
#else
	false
#endif
	;

}

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

namespace sys
{

// Portable Operating System Interface

constexpr long posix_version =
#if defined(_POSIX_VERSION)
	_POSIX_VERSION
#else
	0L
#endif
	;

constexpr long posix2_version =
#if defined(_POSIX2_VERSION)
	_POSIX2_VERSION
#else
	0L
#endif
	;
/*
constexpr long posix2_c_version =
#if defined(_POSIX2_C_VERSION)
	_POSIX2_C_VERSION
#else
	0L
#endif
	;
*/
// Single UNIX Specification

constexpr long xopen_version =
#if defined(_XOPEN_VERSION)
	_XOPEN_VERSION
#else
	0L
#endif
	;

constexpr long xopen_xcu_version =
#if defined(_XOPEN_XCU_VERSION)
	_XOPEN_XCU_VERSION
#else
	0L
#endif
	;

constexpr bool xopen_xpg2 =
#if defined(_XOPEN_XPG2)
	true
#else
	false
#endif
	;

constexpr bool xopen_xpg3 =
#if defined(_XOPEN_XPG3)
	true
#else
	false
#endif
	;

constexpr bool xopen_xpg4 =
#if defined(_XOPEN_XPG4)
	true
#else
	false
#endif
	;

constexpr bool xopen_unix =
#if defined(_XOPEN_UNIX)
	true
#else
	false
#endif
	;

}

//
// POSIX / UNIX / LINUX
//

#if defined(__POSIX__) || defined(__UNIX__) || defined(__LINUX__)
#include <unistd.h>

#ifndef O_BINARY
#define O_BINARY 0L
#endif
#ifndef O_TEXT
#define O_TEXT 0L
#endif

namespace sys
{
	namespace sep
	{
		constexpr auto dir = "/";
		constexpr auto path = ":";
	}

	namespace esc::sh
	{
		constexpr auto first = "$", second = "";
		constexpr auto regex = "$[A-Z_][A-Z_0-9]*";
	}

	using size_t = ::size_t;
	using ssize_t = ::ssize_t;
	using off_t = ::off_t;
	using pid_t = ::pid_t;
	using mode_t = ::mode_t;
	using stat_info = struct stat;

	constexpr auto access = ::access;
	constexpr auto chdir = ::chdir;
	constexpr auto chmod = ::chmod;
	constexpr auto close = ::close;
	constexpr auto creat = ::creat;
	constexpr auto dup = ::dup;
	constexpr auto dup2 = ::dup2;
	constexpr auto execv = ::execv;
	constexpr auto execve = ::execve;
	constexpr auto execvp = ::execvp;
	constexpr auto execl = ::execl;
	constexpr auto execle = ::execle;
	constexpr auto execlp = ::execlp;
	constexpr auto fdopen = ::fdopen;
	constexpr auto fileno = ::fileno;
	constexpr auto fstat = ::fstat;
	constexpr auto getcwd = ::getcwd;
	constexpr auto getpid = ::getpid;
	constexpr auto isatty = ::isatty;
	constexpr auto lseek = ::lseek;
	constexpr auto mkdir = ::mkdir;
	constexpr auto open = ::open;
	constexpr auto pclose = ::pclose;
	constexpr auto pipe = ::pipe;
	constexpr auto popen = ::popen;
	constexpr auto putenv = ::putenv;
	constexpr auto read = ::read;
	constexpr auto rmdir = ::rmdir;
	constexpr auto stat = ::stat;
	constexpr auto swab = ::swab;
	constexpr auto umask = ::umask;
	constexpr auto unlink = ::unlink;
	constexpr auto write = ::write;

} // namespace sys

//
// WIN32
//

#elif defined(__WIN32__) || defined(__OS2__)
#include <io.h>
#include <process.h>
#include <direct.h>

#define F_OK 0
#define W_OK 2
#define R_OK 4

#define O_APPEND _O_APPEND
#define O_BINARY _O_BINARY
#define O_CREAT  _O_CREAT
#define O_EXCL   _O_EXCL
#define O_RDONLY _O_RDONLY
#define O_RDWR   _O_RDWR
#define O_TEXT   _O_TEXT
#define O_TRUNC  _O_TRUNC
#define O_WRONLY _O_WRONLY

namespace sys
{
	namespace sep
	{
		constexpr auto dir = "\\";
		constexpr auto path = ";";
	}

	namespace esc::sh
	{
		constexpr auto first = "%", second = "%";
		constexpr auto regex = "%[A-Z_a-z][A-Z_a-z0-9]*%";
	}

	using size_t = unsigned int;
	using ssize_t = signed int;
	using off_t = long;
	using pid_t = intptr_t;
	using mode_t = int;
	using stat_info = struct _stat;

	constexpr auto access = ::_access;
	constexpr auto chdir = ::_chdir;
	constexpr auto chmod = ::_chmod;
	constexpr auto close = ::_close;
	constexpr auto creat = ::_creat;
	constexpr auto dup = ::_dup;
	constexpr auto dup2 = ::_dup2;
	constexpr auto execl = ::_execl;
	constexpr auto execle = ::_execle;
	constexpr auto execlp = ::_execlp;
	constexpr auto execv = ::_execv;
	constexpr auto execve = ::_execve;
	constexpr auto execvp = ::_execvp;
	constexpr auto fdopen = ::_fdopen;
	constexpr auto fileno = ::_fileno;
	constexpr auto fstat = ::_fstat;
	constexpr auto getcwd = ::_getcwd;
	constexpr auto getpid = ::_getpid;
	constexpr auto isatty = ::_isatty;
	constexpr auto lseek = ::_lseek;
	constexpr auto mkdir = [](char const *dir, mode_t) { return ::_mkdir(dir); };
	constexpr auto open = ::_open;
	constexpr auto pclose = ::_pclose;
	constexpr auto pipe = [](int fd[2]) { return ::_pipe(fd, BUFSIZ, 0); };
	constexpr auto popen = ::_popen;
	constexpr auto putenv = ::_putenv;
	constexpr auto read = ::_read;
	constexpr auto rmdir = ::_rmdir;
	constexpr auto stat = ::_stat;
	constexpr auto swab = ::_swab;
	constexpr auto umask = ::_umask;
	constexpr auto unlink = ::_unlink;
	constexpr auto write = ::_write;

	unsigned long winerr(char const *prefix); // perror for GetLastError

} // namespace sys

//
// Unknown
//

#else
#error Cannot find common system interfaces.
#endif

//
// Common
//

namespace sys
{
	template <typename T> constexpr bool fail(T const t) { return -1 == t; }

	extern char **environment;
	pid_t pexec(int fd[3], char **argv);
	void terminate(pid_t);
};

#endif // file
