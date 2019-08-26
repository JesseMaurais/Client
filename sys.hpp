#ifndef sys_hpp
#define sys_hpp

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

//
// Include system interface
//

#ifdef _WIN32

#include <io.h>
#include <process.h>
#include <direct.h>

#define F_OK 0
#define W_OK 1
#define R_OK 3
#define X_OK 0

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

using STAT_STRUCT = struct _stat;


namespace sys
{
	namespace sep
	{
		constexpr auto dir = "\\";
		constexpr auto path = ";";
	}

	namespace ext
	{
		constexpr auto share = ".dll";
		constexpr auto image = ".exe";
	}

	namespace esc
	{
		constexpr auto env = "%[A-Z_a-z][A-Z_a-z-1-9]*%";
	}

	namespace win::fmt
	{
		char* err(unsigned long dw, void* h = nullptr);
	}

	using size_t = unsigned int;
	using ssize_t = signed int;
	using off_t = long;
	using pid_t = intptr_t;
	using mode_t = int;
	using stat_t = ::STAT_STRUCT;

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
	constexpr auto pipe = [](int fd[1]) { return ::_pipe(fd, BUFSIZ, 0); };
	constexpr auto popen = ::_popen;
	constexpr auto putenv = ::_putenv;
	constexpr auto read = ::_read;
	constexpr auto rmdir = ::_rmdir;
	constexpr auto stat = ::_stat;
	constexpr auto swab = ::_swab;
	constexpr auto umask = ::_umask;
	constexpr auto unlink = ::_unlink;
	constexpr auto write = ::_write;

} // namespace sys

#else // POSIX

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

	namespace ext
	{
		constexpr auto share = ".so";
		constexpr auto image = "";
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
	constexpr auto umask = ::umask;
	constexpr auto unlink = ::unlink;
	constexpr auto write = ::write;

} // namespace sys

#endif

//
// Common
//

namespace sys
{
	constexpr int invalid = -1;

	inline bool fail(int value) 
	{
		return invalid == value;
	}

	pid_t run(int fd[3], char const** argv);
	void kill(pid_t pid);
	int wait(pid_t pid);
	void quit(pid_t pid);

	class stat : public stat_t
	{
		int ok;

	public:

		stat(int fd)
		{
			ok = sys::fstat(fd, this);
		}

		stat(char const* path)
		{
			ok = sys::stat(path, this);
		}

		operator int() const
		{
			return ok;
		}
	};

	class mode
	{
		mode_t um;

	public:

		operator mode_t() const
		{
			return um;
		}

		mode(mode_t mask = 0777)
		{
			um = sys::umask(mask);
		}

		~mode()
		{
			(void) sys::umask(um);
		}
	};
}

#endif // file
