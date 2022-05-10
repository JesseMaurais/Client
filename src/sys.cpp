// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "sys.hpp"
#include "err.hpp"
#include "ptr.hpp"
#ifdef _WIN32
#include "win.hpp"
#else // UNIX
#include "uni.hpp"
#endif

namespace sys
{
	pipe::pipe()
	{
		auto fd = data();
		if (fail(sys::pipe(fd)))
		{
			for (int n : { 0, 1 })
			{
				fd[n] = invalid;
			}
			err(here, "pipe");
		}
	}

	pipe::~pipe()
	{
		auto fd = data();
		for (int n : { 0, 1 })
		{
			if (not fail(fd[n]) and fail(close(fd[n])))
			{
				err(here, "close", fd[n]);
			}
		}
	}

	stats::stats(int fd)
	{
		if (fail(ok = sys::fstat(fd, this)))
		{
			err(here, "fstat");
		}
	}

	stats::stats(const char* path)
	{
		if (fail(ok = sys::stat(path, this)))
		{
			err(here, "state");
		}
	}

	mode::mode(mode_t mask)
	{
		um = sys::umask(mask);
	}

	mode::~mode()
	{
		(void) sys::umask(um);
	}

	char** environ()
	{
		#ifdef _WIN32
		return _environ;
		#else // UNIX
		return ::environ;
		#endif
	}

	pid_t exec(int fd[3], size_t argc, const char** argv)
	{
		#ifdef assert
		assert(nullptr != argv);
		assert(nullptr != argv[0]);
		assert(nullptr == argv[argc]);
		#endif

		for (int n : { 0, 1, 2 })
		{
			fd[n] = invalid;
		}

		#ifdef _WIN32
		{
			win::pipe pair[3];

			for (int n : { 0, 1, 2 })
			{
				if (fail(pair[n].read))
				{
					return -1;
				}

				auto const h = n ? pair[n].read.h : pair[n].write.h;

				if (not SetHandleInformation(h, HANDLE_FLAG_INHERIT, false))
				{
					win::err(here, "SetHandleInformation");
					return invalid;
				}
			}

			char cmd[MAX_PATH];
			ZeroMemory(cmd, sizeof cmd);
			for (size_t i = 0, j = 0; argv[i]; ++i, ++j)
			{
				constexpr auto max = sizeof cmd;
				const auto format = fmt::any_of(argv[i]) ? "\"%s\" " : "%s ";
				const auto n = std::snprintf(cmd + j, max - j, format, argv[i]);
				if (0 < n) j += n - 1;
				else return invalid;
			}

			win::zero<PROCESS_INFORMATION> pi;
			win::size<&STARTUPINFO::cb> si;

			si.dwFlags = STARTF_USESTDHANDLES;
			si.hStdInput = pair[0].read.h;
			si.hStdOutput = pair[1].write.h;
			si.hStdError = pair[2].write.h;

			const bool ok = CreateProcess
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
				win::err(here, "CreateProcess", cmd);
				return invalid;
			}

			const win::handle closed(pi.hThread);

			for (int n : { 0, 1, 2 })
			{
				fd[n] = n ? pair[n].read.open(O_RDONLY) : pair[n].write.open(O_WRONLY);
			}

			return pi.dwProcessId;
		}
		#else
		{
			struct pipe pair[3];

			for (int n : { 0, 1, 2 })
			{
				if (fail(pair[n]))
				{
					err(here, "pipe");
					return invalid;
				}
			}

			if (auto pid = fork(); pid)
			{
				if (fail(pid))
				{
					err(here, "fork");
				}
				else for (int n : { 0, 1, 2 })
				{
					std::swap(fd[n], pair[n][not n]);
				}
				return pid;
			}

			for (int n : { 0, 1, 2 }) try
			{
				if (fail(close(n)))
				{
					throw err(here, "close", n);
				}

				if (fail(dup2(pair[n][not n], n)))
				{
					throw err(here, "dup2", n);
				}

				for (int fd : pair[n])
				{
					if (fail(close(fd)))
					{
						throw err(here, "close", fd);
					}
				}
			}
			catch (int)
			{
				std::abort();
			}

			std::vector<char*> args;
			for (size_t argn = 0; argn < argc; ++argn)
			{
				args.push_back(fwd::non_const(argv[argn]));
			}
			args.push_back(nullptr);

			const int res = execvp(args.front(), args.data());
			sys::err(here, "execvp", res, args.front());
			std::exit(res);
		}
		#endif
	}

	bool kill(pid_t pid)
	{
		#ifdef _WIN32
		{
			constexpr auto dw = PROCESS_ALL_ACCESS;
			const win::handle h = OpenProcess(dw, false, pid);
			if (fail(h))
			{
				win::err(here, "OpenProcess", pid);
				return failure;
			}
			else
			if (not TerminateProcess(h, 0))
			{
				win::err(here, "TerminateProcess", pid);
				return failure;
			}
			return success;
		}
		#else
		{
			if (fail(::kill(pid, SIGTERM)))
			{
				err(here, "SIGTERM", pid);
				return failure;
			}
			return success;
		}
		#endif
	}

	int wait(pid_t pid)
	{
		#ifdef _WIN32
		{
			auto code = static_cast<DWORD>(sys::invalid);
			constexpr auto dw = PROCESS_ALL_ACCESS;
			const win::handle h = OpenProcess(dw, false, pid);
			if (fail(h))
			{
				win::err(here, "OpenProcess", pid);
			}
			else
			{
				if (win::wait(h))
				{
					if (not GetExitCodeProcess(h, &code))
					{
						win::err(here, "GetExitCodeProcess", pid);
					}
				}
			}
			return static_cast<int>(code);
		}
		#else
		{
			int status = -1;
			const pid_t parent = pid;
			do
			{
				pid = waitpid(parent, &status, 0);
				if (fail(pid))
				{
					err(here, "waitpid", parent);
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

	#ifdef _WIN32
	namespace win
	{
		char const* strerr(unsigned long id, void* ptr)
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

			thread_local auto tls = fwd::null_ptr<HLOCAL>(LocalFree);

			LPSTR str = nullptr;
			auto addr = (LPSTR) &str;
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

			if (0 < size) // replace
			{
				tls.reset(reinterpret_cast<HLOCAL>(str));
			}

			if (nullptr == str)
			{
				thread_local char buf[128] = { '\0' };
				std::sprintf(buf, "Unknown error %lu", id);
				str = buf;
			}

			return str;
		}
	}
	#else //POSIX
	namespace uni
	{
		char const* strerr(int no)
		{
			thread_local char buf[128] = { '\0' };
			// POSIX and GNU differ on return type
			(void) strerror_r(no, buf, sizeof buf);
			return buf;
		}
	}
	#endif
}
