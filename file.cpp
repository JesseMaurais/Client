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

	ssize_t descriptor::write(const void* buffer, size_t size)
	{
		ssize_t const n = sys::write(fd, buffer, size);
		if (-1 == n)
		{
			sys::perror("write", fd, size);
		}
		return n;
	}

	ssize_t descriptor::read(void* buffer, size_t size)
	{
		ssize_t const n = sys::read(fd, buffer, size);
		if (-1 == n)
		{
			sys::perror("read", fd, size);
		}
		return n;
	}

	descriptor::~descriptor()
	{
		if (-1 != fd)
		{
			if (-1 == sys::close(fd))
			{
				sys::perror("close", fd);
			}
		}
	}

	pipe::pipe()
	{
		int pair[2];
		if (-1 == sys::pipe(pair))
		{
			sys::perror("pipe");
			return;
		}
		for (int i : { 0, 1 })
		{
			fd[i].set(pair[i]);
		}
	}

	void process::open(arguments args, openmode mode)
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

			si.cb = sizeof si;
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

			id = static_cast<std::intptr_t>(pi.hProcess);
			fd[STDIN_FILENO] = out.read.release();
			fd[STDOUT_FILENO] = in.write.release();
		}
		else
		#endif // WIN32
		if constexpr (sys::POSIX)
		{
			sys::file::pipe pair[2];
			if (pair[0] or pair[1])
			{
				return;
			}
			pid = sys::fork();
			if (-1 == pid)
			{
				sys::perror("fork");
			}
			else
			if (0 == pid)
			{
				auto const no = { STDIN_FILENO, STDOUT_FILENO };

				for (int i : no)
				{
					int j = pair[i][i].get();
					if (-1 == sys::dup2(j, i))
					{
						sys::perror("dup2", j, i);
						std::exit(EXIT_FAILURE);
					}
				}

				for (int i : no)
				{
					for (int j : no)
					{
						int k = pair[i][j].set();
						if (-1 == sys::close(k))
						{
							sys::perror("close", k);
							std::exit(EXIT_FAILURE);
						}
					}
				}

				std::vector<char*> cmds;
				for (auto s : args) cmds.push_back(const_cast<char*>(s));
				cmds.push_back(nullptr);

				int const result = sys::execvp(cmds.front(), cmds.data());
				sys::perror("execv", cmds.front());
				std::exit(result);
			}
			else
			{
				fd[0].set(pair[0][1].set());
				fd[1].set(pair[1][0].set());
			}
		}
	}
}

