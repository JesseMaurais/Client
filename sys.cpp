// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "sys.hpp"
#include "err.hpp"
#include "pos.hpp"
#include "ptr.hpp"
#include "file.hpp"
#include <vector>

namespace sys
{
	#ifdef _WIN32
	namespace win::fmt
	{
		char* err(unsigned long id, void* ptr)
		{
			constexpr auto flag = FORMAT_MESSAGE_ALLOCATE_BUFFER
			                    | FORMAT_MESSAGE_IGNORE_INSERTS
		    	                | FORMAT_MESSAGE_FROM_HMODULE
			                    | FORMAT_MESSAGE_FROM_SYSTEM;

			auto h = static_cast<HANDLE>(ptr);
			if (sys::win::fail(h))
			{
				h = GetModuleHandle(nullptr);
			}
			
			static thread_local auto tls = null_ptr<HLOCAL>(LocalFree);

			LPSTR const str = nullptr;
			auto const addr = (LPSTR) &str;
			auto const size = FormatMessage
			(
				flag,   // style
				h,      // module
				id,     // message
				0,      // language
				addr,   // buffer
				0,      // size
				nullptr // arguments
			);
		
			if (0 < size)
			{
				tls.reset((HLOCAL) str);
			}
			return str;
		}
	}
	#else
	namespace uni::fmt
	{
		char* err(int no)
		{
			static thread_local char buf[64] = { '\0' };
			(void) strerror_r(no, buf, sizeof buf);
			return buf;
		}
	}
	#endif

	pid_t run(int fd[3], char const** argv)
	{
		#ifdef _WIN32
		{
			sys::win::pipe pair[3];

			for (int n : { 0, 1, 2 })
			{
				if (fail(pair[n].read))
				{
					return -1;
				}

				auto const h = n ? pair[n].read.h : pair[n].write.h;

				if (not SetHandleInformation(h, HANDLE_FLAG_INHERIT, false))
				{
					sys::win::err("SetHandleInformation");
					return sys::invalid;
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

			sys::win::zero<PROCESS_INFORMATION> pi;
			sys::win::size<STARTUPINFO, &STARTUPINFO::cb> si;

			si.dwFlags = STARTF_USESTDHANDLES;
			si.hStdInput = pair[0].read.h;
			si.hStdOutput = pair[1].write.h;
			si.hStdError = pair[2].write.h;

			bool const ok = CreateProcess
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
				sys::win::err(here, "CreateProcess", cmd);
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
				if (fail(p))
				{
					return sys::invalid;
				}
			}

			pid_t const pid = fork();
			if (pid)
			{
				if (fail(pid))
				{
					sys::err(here, "fork");
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
					exit(EXIT_FAILURE);
				}

				for (int j : { 0, 1 })
				{
					k = pair[i][j].set();

					if (fail(close(k)))
					{
						exit(EXIT_FAILURE);
					}
				}
			}

			std::vector<char*> args;
			for (int i = 0; argv[i]; ++i)
			{
				args.push_back(const_cast<char*>(argv[i]));
			}
			args.push_back(nullptr);

			int const res = execvp(args.front(), args.data());
			sys::err(here, "execvp", args.front());
			std::exit(res);
		}
		#endif
	}

	void kill(pid_t pid)
	{
		#ifdef _WIN32
		{
			sys::win::process const h(pid);
			if (fail(h))
			{
				sys::warn(here, pid);
			}
			else
			if (not TerminateProcess(h, 0))
			{
				sys::win::err(here, "TerminateProcess", pid);
			}
		}
		#else
		{
			if (fail(::kill(pid, SIGTERM)))
			{
				sys::err(here, "SIGTERM", pid);
			}
		}
		#endif
	}

	void quit(pid_t pid)
	{
		#ifdef _WIN32
		{
			auto const dw = static_cast<DWORD>(pid);

			sys::win::threads it;
			if (not fail(it.snap)) do
			{
				if (it.th32OwnerProcessID == dw)
				{
					if (sys::win::msg::quit(it.th32ThreadID))
					{
						sys::err(here, here);
					}
				}
			}
			while (++it);
		}
		#else
		{
			if (fail(::kill(pid, SIGINT)))
			{
				sys::err(here, "SIGINT", pid);
			}
		}
		#endif
	}

	int wait(pid_t pid)
	{
		#ifdef _WIN32
		{
			auto code = static_cast<DWORD>(sys::invalid);
			sys::win::process const h(pid);
			if (fail(h))
			{
				sys::warn(here, pid);
			}
			else
			{
				if (sys::win::wait(h))
				{
					if (not GetExitCodeProcess(h, &code))
					{
						sys::win::err(here, "GetExitCodeProcess", pid);
					}
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
				if (fail(pid))
				{
					sys::err(here, "waitpid", parent);
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

