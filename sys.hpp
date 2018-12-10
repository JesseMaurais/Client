#ifndef sys_hpp
#define sys_hpp

//
// Common
//

#include <sys/types.h>
#include <sys/stat.h>
#include <search.h>
#include <stdlib.h>
#include <stdio.h>

//
// POSIX
//

#if __has_include(<unistd.h>)
#include <unistd.h>
#include <fcntl.h>

namespace sys
{

// Portable Operating System Interface
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
constexpr bool POSIX = POSIX_VERSION > 0;
constexpr bool WINRT = false;

#ifndef O_BINARY
#define O_BINARY 0L
#endif
#ifndef O_TEXT
#define O_TEXT 0L
#endif

using size_t = ::size_t;
using ssize_t = ::ssize_t;
using off_t = ::off_t;
using pid_t = ::pid_t;
using mode_t = ::mode_t;
using statbuf = struct ::stat;

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
constexpr auto execvpe = ::execvpe;
constexpr auto execl = ::execl;
constexpr auto execle = ::execle;
constexpr auto execlp = ::execlp;
constexpr auto fdopen = ::fdopen;
constexpr auto fileno = ::fileno;
constexpr auto fork = ::fork;
constexpr auto fstat = ::fstat;
constexpr auto getcwd = ::getcwd;
constexpr auto getpid = ::getpid;
constexpr auto isatty = ::isatty;
constexpr auto lfind = ::lfind;
constexpr auto lsearch = ::lsearch;
constexpr auto lseek = ::lseek;
constexpr auto mkdir = ::mkdir;
constexpr auto mktemp = ::mktemp;
constexpr auto open = ::open;
constexpr auto pclose = ::pclose;
constexpr auto pipe = ::pipe;
constexpr auto popen = ::popen;
constexpr auto putenv = ::putenv;
constexpr auto read = ::read;
constexpr auto rmdir = ::rmdir;
constexpr auto stat = ::stat;
constexpr auto swap = ::swab;
constexpr auto umask = ::umask;
constexpr auto unlink = ::unlink;
constexpr auto vfork = ::vfork;
constexpr auto write = ::write;


} // namespace sys

#else

//
// MSVCRT
//

#if __has_include(<io.h>)
#include <io.h>
#include <errno.h>
#include <direct.h>
#include <process.h>

namespace sys
{

constexpr bool WINRT =
#ifdef _WINRT_DLL
	false
#else
	true
#endif
	;

constexpr bool XSI = false;
constexpr long XOPEN_VERSION = 0;
constexpr bool POSIX = false;
constexpr long POSIX_VERSION = 0;

constexpr int _nosys = [] { ::_set_errno(ENOSYS); return -1; };

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#define F_OK 0
#define W_OK 2
#define R_OK 4

#define O_APPEND _O_APPEND
#define O_BINARY _O_BINARY
#define O_CREAT  _O_CREAT
#define O_EXCL   _O_NOINHERIT
#define O_RDONLY _O_RDONLY
#define O_RDWR   _O_RDWR
#define O_TEXT   _O_TEXT
#define O_TRUNC  _O_TRUNC
#define O_WRONLY _O_WRONLY

using size_t = unsigned int;
using ssize_t = signed int;
using off_t = long;
using pid_t = int;
using mode_t = int;
using statbuf = struct ::_stat;

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
constexpr auto execvpe = ::_execvpe;
constexpr auto fdopen = ::_fdopen;
constexpr auto fileno = ::_fileno;
constexpr auto fork = [] { return _nosys() };
constexpr auto fstat = ::_fstat;
constexpr auto getcwd = ::_getcwd;
constexpr auto getpid = ::_getpid;
constexpr auto isatty = ::_isatty;
constexpr auto lfind = ::_lfind;
constexpr auto lsearch = ::_lsearch;
constexpr auto lseek = ::_lseek;
constexpr auto mkdir = [](const char *dir, mode_t) { return ::_mkdir(dir); };
constexpr auto mktemp = ::_mktemp;
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
constexpr auto vfork = [] { return _nosys(); };
constexpr auto write = ::_write;

} // namespace sys

#else

//
// Unknown
//

#error Cannot find system headers
#endif // MSVC
#endif // POSIX

//
// Common
//

namespace sys
{
	pid_t pexec(int fd[3], char **argv);
};

#endif // file
