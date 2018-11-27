#include "file.hpp"
#include "sys.hpp"
#include "err.hpp"
#include "os.hpp"

#if __has_include(<processthreadapi.h>)
#include <processthreadapi.h>
#endif

namespace
{
	using namespace sys::file;
	int convert(openmode mode)
	{
		int flags = 0;
		if (mode & in and mode & out)
		{
			flags |= O_RDWR;
		}
		else if (mode & out)
		{
			flags |= O_WRONLY;
		}
		else if (mode & in)
		{
			flags |= O_RDONLY;
		}
		if (mode & bin)
		{
			flags |= O_BINARY;
		}
		if (mode & app)
		{
			flags |= O_APPEND;
		}
		if (mode & trunc)
		{
			flags |= O_TRUNC;
		}
		if constexpr (sys::LINUX)
		{
			flags |= O_DIRECT;
		}
		return flags;
	}
}

namespace sys::file
{
	void descriptor::open(std::string const& path, openmode mode)
	{
		fd = sys::open(path.c_str(), convert(mode));
		if (-1 == fd)
		{
			sys::perror("open");
		}
	}

	descriptor::~descriptor()
	{
		if (-1 != fd)
		{
			if (sys::close(fd))
			{
				sys::perror("close", fd);
			}
		}
	}

	pipe::pipe()
	{
		int fd[2];
		if (sys::pipe(fd))
		{
			sys::perror("pipe");
			return;
		}
		this->fd[0] = fd[0];
		this->fd[1] = fd[1];
	}

	void pipe::open(std::initializer_list<char*> args, openmode mode)
	{
		#ifdef __WIN32__
		if constexpr (sys::WIN32)
		{
			auto const cmds = fmt::join(args, " ");

			struct Handle
			{
				HANDLE h = nullptr;

				~Handle()
				{
					if (h) CloseHandle(h);
				}

				int release()
				{
					int fd = _open_osfhandle(h, 0);
					h = nullptr;
					return fd;
				}

				operator bool()
				{
					return h;
				}
			};

			struct Pipe
			{
				Handle read;
				Handle write;
				BOOL ok;

				Pipe()
				{
					SECURITY_ATTRIBUTES sa;
					ZeroMemory(&sa, sizeof sa);
					sa.nLength = sizeof sa;
					sa.bInheritHandle = TRUE;
					ok = CreatePipe
					(
						&read.h,
						&write.h,
						&sa,
						BUFSIZ
					);
				}

				operator bool()
				{
					return ok;
				}

			} in, out;

			if (not in or not out)
			{
				return;
			}

			for (HANDLE h : { in.write.h, out.read.h })
			{
				if (not SetHandleInformation(&h, HANDLE_FLAG_INHERIT, 0))
				{
					return;
				}
			}

			PROCESS_INFORMATION pi;
			ZeroMemory(&pi, sizeof pi);

			STARTUPINFO si;
			ZeroMemory(&si, sizeof si);

			si.cb = sizeof sa;
			si.dwFlags = STARTF_USESTDHANDLES;
			si.hStdIn = in.read.h;
			si.hStdOut = out.write.h;

			BOOL const ok = CreateProcess
			(
				args.front(), // application
				cmds.data(),  // command line
				NULL,         // process attributes
				NULL,         // thread attributes
				TRUE,         // inherit handles
				0,            // creation flags
				NULL,         // environment
				NULL,         // current directory
				&si,          // start-up info
				&pi           // process info
			);

			if (not ok)
			{
				return;
			}

			fd[STDIN_FILENO] = out.read.release();
			fd[STDOUT_FILENO] = in.write.release();
		}
		else
		#endif // WIN32
		if constexpr (sys::POSIX)
		{
			sys::file::pipe in, out;
			if (not in or not out)
			{
				return;
			}
			sys::pid_t const pid = sys::fork();
			if (-1 == pid)
			{
				sys::perror("fork");
			}
			else
			if (0 == pid)
			{
				if (-1 == sys::dup2(in[STDIN_FILENO], STDIN_FILENO))
				{
					sys::perror("dup2", in[STDIN_FILENO], STDIN_FILENO);
					return;
				}
				if (-1 == sys::dup2(out[STDOUT_FILENO], STDOUT_FILENO))
				{
					sys::perror("dup2", out[STDOUT_FILENO], STDOUT_FILENO);
					return;
				}

				std::vector<char*> cmds(args);
				cmds.push_back(nullptr);

				if (-1 == sys::execv(cmds.front(), cmds.data()))
				{
					sys::perror("execv");
				}
				std::exit(EXIT_FAILURE);
			}
			else
			{
				fd[STDIN_FILENO] = out.release(STDOUT_FILENO);
				fd[STDOUT_FILENO] = in.release(STDIN_FILENO);
			}
		}
	}
}

