// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "sys.hpp"
#include "env.hpp"
#include "err.hpp"
#include "file.hpp"
#include <iostream>
#include <signal.h>

#if __has_include(<windows.h>)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#if __has_include(<processthreadapi.h>)
#include <processthreadapi.h>
#endif

#if __has_include(<sys/wait.h>)
#include <sys/wait.h>
#endif

namespace sys
{
	#if defined(__WIN32__)

	unsigned long winerr(char const *prefix)
	{
		LPSTR data = nullptr;
		LPSTR address = reinterpret_cast<LPSTR>(&data);
		constexpr DWORD lang = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
		DWORD const code = GetLastError();
		DWORD const size = FormatMessageA
		(
		 FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		 nullptr, // source
		 code,    // message
		 lang,    // language
		 address, // buffer
		 0,       // size
		 nullptr  // arguments
		);
		if (data)
		{
			fmt::string_view msg(data, size);
			std::cerr << prefix << ": " << msg << std::endl;
			LocalFree(data);
		}
		return code;
	}

	int handle::open(int flags)
	{
		auto const ptr = static_cast<intptr_t>(h);
		int const fd = _open_osfhandle(ptr, flags);
		h = nullptr;
		return fd;
	}

	handle::~handle()
	{
		if (h and not CloseHandle(h))
		{
			winerr("CloseHandle");
		}
	}

	winpipe::winpipe()
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

		if (not ok)
		{
			winerr("CreatePipe");
		}
	}

	#endif // __WIN32__


	pid_t exec(int fd[3], char const**argv)
	{
		#if defined(__WIN32__)
		{
			winpipe pair[3];

			for (int n : { 0, 1, 2 })
			{
				if (not pair[n].ok)
				{
					return -1;
				}

				HANDLE h = n ? pair[n].read.h : pair[n].write.h;

				if (not SetHandleInformation(h, HANDLE_FLAG_INHERIT, false))
				{
					winerr("SetHandleInformation");
					return -1;
				}
			}

			char cmd[MAX_PATH];
			ZeroMemory(cmd, sizeof cmd);
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
			 nullptr,          // application
			 cmd,              // command line
			 nullptr,          // process attributes
			 nullptr,          // thread attributes
			 true,             // inherit handles
			 DETACHED_PROCESS, // creation flags
			 nullptr,          // environment
			 nullptr,          // current directory
			 &si,              // start-up info
			 &pi               // process info
			);

			if (not ok)
			{
				winerr("CreateProcess");
				return -1;
			}

			handle scoped(pi.hThread); // close

			for (int n : { 0, 1, 2 })
			{
				fd[n] = n ? pair[n].read.open(_O_RDONLY) : pair[n].write.open(_O_WRONLY);
			}

			return pi.dwProcessId;
		}
		#else // defined(__POSIX__)
		{
			sys::file::pipe pair[3];

			for (auto const& p : pair)
			{
				if (not p) return -1;
			}

			pid_t const pid = fork();
			if (pid)
			{
				if (fail(pid))
				{
					perror("fork");
				}
				else for (int i : { 0, 1, 2 })
				{
					fd[i] = pair[i][0 == i].set();
				}
				return pid;
			}

			for (int i : { 0, 1, 2 })
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

			std::vector<char*> args;
			for (int i = 0; argv[i]; ++i) args.push_back(const_cast<char*>(argv[i]));
			args.push_back(nullptr);

			int const res = execvp(args.front(), args.data());
			perror("execvp");
			std::exit(res);
		}
		#endif
	}

	void term(pid_t pid)
	{
		#if defined(__WIN32__)
		{
			handle const h = OpenProcess(PROCESS_ALL_ACCESS, true, pid);
			if (nullptr == h)
			{
				winerr("OpenProcess");
			}
			else
			if (not TerminateProcess(h, 0))
			{
				winerr("TerminateProcess");
			}
		}
		#else // defined(__POSIX__)
		{
			if (not fail(pid) and fail(kill(pid, SIGTERM)))
			{
				sys::perror("kill", pid);
			}
		}
		#endif
	}

	int wait(pid_t pid)
	{
		#if defined(__WIN32__)
		{
			DWORD code = ~DWORD{0};
			handle const h = OpenProcess(PROCESS_ALL_ACCESS, true, pid);
			if (nullptr == h)
			{
				winerr("OpenProcess");
			}
			else
			{
				if (WaitForSingleObject(h, INFINITE) == WAIT_FAILED)
				{
					winerr("WaitForSingleObject");
				}
				else
				if (not GetExitCodeProcess(h, &code))
				{
					winerr("GetExitCodeProcess");
				}
			}
			return code;
		}
		#else // defined(__POSIX__)
		{
			int status = -1;
			pid_t const parent = pid;
			do
			{
				pid = waitpid(parent, &status, 0);
				if (fail(pid))
				{
					perror("waitpid");
				}
			}
			while (pid != parent);

			if (WIFEXITED(status))
			{
				return WEXITSTATUS(status);
			}
			else
			if (WIFSTOPPED(status))
			{
				return WSTOPSIG(status);
			}
			else
			if (WIFSIGNALED(status))
			{
				return WTERMSIG(status);
			}

			return status; // unreachable?
		}
		#endif
	}
}

