#include "sys.hpp"
#include "file.hpp"

#if __has_include(<windows.h>)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#if __has_include(<processthreadapi.h>)
#include <processthreadapi.h>
#endif

namespace sys
{
	pid_t pexec(int fd[3], char **argv)
	{
		auto const stdno = { STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO };

		#if defined(__WIN32__)
		{
			struct Handle
			{
				HANDLE h = nullptr;

				~Handle()
				{
					if (h) CloseHandle(h);
				}

				int set()
				{
					auto ptr = std::intptr_t(h);
					int fd = _open_osfhandle(ptr, 0);
					h = nullptr;
					return fd;
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

			} pair[3];

			for (int i : stdno)
			{
				if (not pair[i].ok)
				{
					return -1;
				}

				HANDLE h = i ? pair[i].read.h : pair[i].write.h;

				if (not SetHandleInformation(h, HANDLE_FLAG_INHERIT, 0))
				{
					return -1;
				}
			}

			char cmd[MAX_PATH];
			for (size_t i = 0, j = 0; argv[i]; ++i, ++j)
			{
				int n = std::snprintf(cmd + j, sizeof cmd - j, "%s ", argv[i]);
				if (0 < n) j += n;
				else return -1;
			}

			PROCESS_INFORMATION pi;
			ZeroMemory(&pi, sizeof pi);

			STARTUPINFO si;
			ZeroMemory(&si, sizeof si);

			si.cb = sizeof si;
			si.dwFlags = STARTF_USESTDHANDLES;
			si.hStdInput = pair[0].read.h;
			si.hStdOutput = pair[1].write.h;
			si.hStdError = pair[2].write.h;

			BOOL const ok = CreateProcessA
			(
				argv[0], // application
				cmd,     // command line
				NULL,    // process attributes
				NULL,    // thread attributes
				TRUE,    // inherit handles
				NULL,    // creation flags
				NULL,    // environment
				NULL,    // current directory
				&si,     // start-up info
				&pi      // process info
			);

			if (not ok)
			{
				return -1;
			}

			for (int i : stdno)
			{
				fd[i] = i ? pair[i].read.set() : pair[i].write.set();
			}

			return std::intptr_t(pi.hProcess);
		}
		#elif defined(__POSIX__)
		{
			sys::file::pipe pair[3];
			if (pair[0] or pair[1] or pair[2])
			{
				return -1;
			}

			pid_t const pid = fork();
			if (pid)
			{
				if (fail(pid))
				{
					perror("fork");
				}
				else for (int i : stdno)
				{
					fd[i] = pair[i][0 == i].set();
				}
				return pid;
			}

			for (int i : stdno)
			{
				int k = pair[i][0 != i].get();

				if (fail(close(i)) or fail(dup2(k, i)))
				{
					std::exit(EXIT_FAILURE);
				}

				for (int j : { 0, 1 })
				{
					k = pair[i][j].set();

					if (fail(close(k)))
					{
						std::exit(EXIT_FAILURE);
					}
				}
			}

			int const res = execvp(argv[0], argv);
			perror("execvp");
			std::exit(res);
		}
		#endif
	}
}

