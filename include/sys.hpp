#ifndef sys_hpp
#define sys_hpp "System Interface"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>

#ifdef _MSC_VER

#include <process.h>
#include <direct.h>
#include <io.h>

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#define F_OK 0
#define W_OK 1
#define R_OK 3
#define X_OK 0 // no-op

#define O_APPEND _O_APPEND
#define O_BINARY _O_BINARY
#define O_CREAT  _O_CREAT
#define O_EXCL   _O_EXCL
#define O_RDONLY _O_RDONLY
#define O_RDWR   _O_RDWR
#define O_TEXT   _O_TEXT
#define O_TRUNC  _O_TRUNC
#define O_WRONLY _O_WRONLY

#define S_IRUSR  _S_IREAD
#define S_IWUSR  _S_IWRITE
#define S_IXUSR  _S_IEXEC
#define S_IRGRP  _S_IREAD
#define S_IWGRP  _S_IWRITE
#define S_IXGRP  _S_IEXEC
#define S_IROTH  _S_IREAD
#define S_IWOTH  _S_IWRITE
#define S_IXOTH  _S_IEXEC
#define S_IRWXU ( S_IRUSR | S_IWUSR | S_IXUSR )
#define S_IRWXG ( S_IRGRP | S_IWGRP | S_IXGRP )
#define S_IFMT   _S_IFMT
#define S_IFDIR  _S_IFDIR
#define S_IFCHR  _S_IFCHR
#define S_IFREG  _S_IFREG
#define S_IFIFO  _S_IFIFO

#define S_ISFIFO(x) (((x) & S_IFMT) == S_IFIFO)
#define S_ISDIR(x)  (((x) & S_IFMT) == S_IFDIR)
#define S_ISREG(x)  (((x) & S_IFMT) == S_IFREG)
#define S_ISCHR(x)  (((x) & S_IFMT) == S_IFCHR)

namespace sys
{
	namespace tag
	{
		constexpr auto dir = "\\";
		constexpr auto path = ";";
		constexpr auto share = ".dll";
		constexpr auto ext = ".exe";
	}

	using size_t = unsigned int;
	using ssize_t = signed int;
	using off_t = long;
	using pid_t = intptr_t;
	using mode_t = int;
	using stat_t = struct _stat;

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
	constexpr auto tempnam = ::_tempnam;
	constexpr auto umask = ::_umask;
	constexpr auto unlink = ::_unlink;
	constexpr auto write = ::_write;
}

#else // POSIX

#include <unistd.h>
#include <sys/wait.h>

#ifndef O_BINARY
#define O_BINARY 0L
#endif
#ifndef O_TEXT
#define O_TEXT 0L
#endif

namespace sys
{
	namespace tag
	{
		constexpr auto dir = "/";
		constexpr auto path = ":";
		constexpr auto share = ".so";
		constexpr auto ext = "";
	}

	namespace uni
	{
		const char* strerr(int no);
	}

	using size_t = ::size_t;
	using ssize_t = ::ssize_t;
	using off_t = ::off_t;
	using pid_t = ::pid_t;
	using mode_t = ::mode_t;
	using stat_t = struct stat;

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
	constexpr auto getppid = ::getppid;
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
	constexpr auto tempnam = ::tempnam;
	constexpr auto umask = ::umask;
	constexpr auto unlink = ::unlink;
	constexpr auto write = ::write;
}

#endif // OS

using stat_t =
#ifdef _WIN32
	struct _stat;
#else
	struct stat;
#endif

#include <array>

namespace sys
{
	constexpr int invalid = -1;

	inline bool fail(int value)
	{
		return invalid == value;
	}

	struct pipe : std::array<int, 2>
	{
		pipe(), ~pipe();
	};

	inline bool fail(const struct pipe &fd)
	{
		return fail(fd[0]) or fail(fd[1]);
	}

	struct stats : ::stat_t
	{
		stats(int), stats(const char* path);

		int ok;
	};

	struct mode
	{
		mode(mode_t mask = 0777), ~mode();

		mode_t um;
	};

	char** environment();
	pid_t exec(int fd[3], int argc, char **argv);
	bool kill(pid_t);
	int wait(pid_t);
}

#endif // file
