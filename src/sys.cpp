// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "err.hpp"
#include "sys.hpp"
#include "ptr.hpp"

#ifdef _WIN32
#include "win.hpp"
#else
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
			perror("pipe");
		}
	}

	pipe::~pipe()
	{
		auto fd = data();
		for (int n : { 0, 1 })
		{
			if (not fail(fd[n]) and fail(close(fd[n])))
			{
				perror("close");
			}
		}
	}

	stats::stats(int fd)
	{
		if (fail(ok = fstat(fd, this)))
		{
			perror("fstat");
		}
	}

	stats::stats(const char* path)
	{
		if (fail(ok = lstat(path, this)))
		{
			perror("lstat");
		}
	}

	mode::mode(mode_t mask)
	{
		um = umask(mask);
	}

	mode::~mode()
	{
		(void) umask(um);
	}

	char** environment()
	{
		#ifdef _WIN32
		return _environ;
		#else
		return ::environ;
		#endif
	}

	pid_t exec(int fd[3], int argc, char** argv)
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

				auto const h = n ? pair[n].read.get() : pair[n].write.get();

				if (not SetHandleInformation(h, HANDLE_FLAG_INHERIT, false))
				{
					#ifdef WINERR
					WINERR("SetHandleInformation");
					#endif
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
			si.hStdInput = pair[0].read.get();
			si.hStdOutput = pair[1].write.get();
			si.hStdError = pair[2].write.get();

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
				#ifdef WINERR
				WINERR("CreateProcess", cmd);
				#endif
				return invalid;
			}

			const win::handle closed(pi.hThread);

			for (int n : { 0, 1, 2 })
			{
				fd[n] = n
					? sys::win::open(pair[n].read.get(), O_RDONLY)
					: sys::win::open(pair[n].write.get(), O_WRONLY);
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
					perror("pipe");
					return invalid;
				}
			}

			if (auto pid = fork(); pid)
			{
				if (fail(pid))
				{
					perror("fork");
				}
				else for (int n : { 0, 1, 2 })
				{
					std::swap(fd[n], pair[n][not n]);
				}
				return pid;
			}

			for (int n : { 0, 1, 2 })
			{
				if (fail(close(n)))
				{
					perror("close");
				}

				if (fail(dup2(pair[n][not n], n)))
				{
					perror("dup2");
				}

				for (int fd : pair[n])
				{
					if (fail(close(fd)))
					{
						perror("close");
					}
				}
			}

			const int code = execvp(argv[0], argv);
			perror("execvp");
			std::exit(code);
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
				#ifdef WINERR
				WINERR("OpenProcess");
				#endif
				return failure;
			}
			else
			if (not TerminateProcess(h, 0))
			{
				#ifdef WINERR
				WINERR("TerminateProcess");
				#endif
				return failure;
			}
			return success;
		}
		#else
		{
			if (fail(::kill(pid, SIGTERM)))
			{
				perror("SIGTERM");
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
				#ifdef WINERR
				WINERR("OpenProcess");
				#endif
			}
			else
			{
				if (win::wait(h))
				{
					if (not GetExitCodeProcess(h, &code))
					{
						#ifdef WINERR
						WINERR("GetExitCodeProcess");
						#endif
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

#ifdef _WIN32
namespace sys::win
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

		thread_local auto tls = fwd::null_unique<fwd::no_ptr<HLOCAL>>(LocalFree);

		LPSTR str = nullptr;
		auto data = (LPSTR) &str;
		const auto size = FormatMessage
		(
			flag,   // style
			h,      // module
			id,     // message
			0,      // language
			data,   // buffer
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
#else
namespace sys::uni
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
