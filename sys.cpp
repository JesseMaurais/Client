// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "sys.hpp"
#include "env.hpp"
#include "err.hpp"
#include "file.hpp"

#ifdef _WIN32
# include "win.hpp"
# include <iostream>
#else
# include <sys/wait.h>
# include <signal.h>
#endif

namespace sys
{
	#ifdef _WIN32

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

	#endif

	pid_t run(int fd[3], char const**argv)
	{
		#ifdef _WIN32
		{
			sys::win::pipe pair[3];

			for (int n : { 0, 1, 2 })
			{
				if (not pair[n].ok)
				{
					return -1;
				}

				auto const h = n ? pair[n].read.h : pair[n].write.h;

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

			sys::win::process_info pi;
			sys::win::startup_info si;

			si.dwFlags = STARTF_USESTDHANDLES;
			si.hStdInput = pair[0].read.h;
			si.hStdOutput = pair[1].write.h;
			si.hStdError = pair[2].write.h;

			bool const ok = CreateProcessA
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
				sys::win::perror(here, "CreateProcess", cmd);
				return sys::invalid;
			}

			sys::win::handle const closed(pi.hThread);

			for (int n : { 0, 1, 2 })
			{
				fd[n] = n ? pair[n].read.open(O_RDONLY) : pair[n].write.open(O_WRONLY);
			}

			return pi.dwProcessId;
		}
		#else
		{
			sys::file::pipe pair[3];

			for (auto const& p : pair)
			{
				if (not p) return -1;
			}

			pid_t const pid = fork();
			if (pid)
			{
				if (sys::fail(pid))
				{
					sys::perror(here, "fork");
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

				if (sys::fail(close(i)) or sys::fail(dup2(k, i)))
				{
					std::exit(EXIT_FAILURE);
				}

				for (int j : { 0, 1 })
				{
					k = pair[i][j].set();

					if (sys::fail(close(k)))
					{
						std::exit(EXIT_FAILURE);
					}
				}
			}

			std::vector<char*> args;
			for (int i = 0; argv[i]; ++i) args.push_back(const_cast<char*>(argv[i]));
			args.push_back(nullptr);

			int const res = execvp(args.front(), args.data());
			sys::perror(here, "execvp", args.front());
			std::exit(res);
		}
		#endif
	}

	void kill(pid_t pid)
	{
		#ifdef _WIN32
		{
			sys::win::handle const h = OpenProcess(PROCESS_ALL_ACCESS, true, pid);
			if (sys::win::fail(h))
			{
				sys::win::perror(here, "OpenProcess", pid);
			}
			else
			if (not TerminateProcess(h, 0))
			{
				sys::win::perror(here, "TerminateProcess", pid);
			}
		}
		#else
		{
			bool const ok = not sys::fail(pid);
			if (ok and sys::fail(::kill(pid, SIGTERM)))
			{
				sys::perror(here, "kill", pid);
			}
		}
		#endif
	}

	int wait(pid_t pid)
	{
		#ifdef _WIN32
		{
			DWORD code = static_cast<DWORD>(sys::invalid);
			sys::win::handle const h = OpenProcess(PROCESS_ALL_ACCESS, true, pid);
			if (sys::win::fail(h))
			{
				sys::win::perror(here, "OpenProcess", pid);
			}
			else
			{
				if (WaitForSingleObject(h, INFINITE) == WAIT_FAILED)
				{
					sys::win::perror(here, "WaitForSingleObject", pid);
				}
				else
				if (not GetExitCodeProcess(h, &code))
				{
					sys::win::perror(here, "GetExitCodeProcess", pid);
				}
			}
			return static_cast<int>(code);
		}
		#else
		{
			int status = -1;
			pid_t const parent = pid;
			do
			{
				pid = waitpid(parent, &status, 0);
				if (sys::fail(pid))
				{
					sys::perror(here, "waitpid", parent);
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

