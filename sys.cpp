// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "sys.hpp"
#include "str.hpp"
#include "err.hpp"
#include "file.hpp"
#include <iostream>
#include <signal.h>

#if defined(__WIN32__)
extern char **_environ;
namespace sys
{
	char **environment = _environ;
}
#else // defined(__POSIX__)
extern char **environ;
namespace sys
{
	char **environment = ::environ;
}
#endif

#if __has_include(<io.h>)
#include <io.h>
#endif

#if __has_include(<windows.h>)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#if __has_include(<processthreadapi.h>)
#include <processthreadapi.h>
#endif

#if __has_include(<sys/mman.h>)
#include <sys/mman.h>
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
		 FORMAT_MESSAGE_ALLOCATE_BUFFER	| FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
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

	struct Handle
	{
		HANDLE h;

		Handle(HANDLE h = nullptr)
		{
			this->h = h;
		}

		~Handle()
		{
			if (h and not CloseHandle(h))
			{
				winerr("CloseHandle");
			}
		}

		int set(int flags)
		{
			auto const ptr = reinterpret_cast<std::intptr_t>(h);
			int const fd = _open_osfhandle(ptr, flags);
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

			if (not ok)
			{
				winerr("CreatePipe");
			}
		}
	};
	
	#endif // __WIN32__

	pid_t exec(int fd[3], char **argv)
	{
		#if defined(__WIN32__)
		{
			Pipe pair[3];

			for (int n : { 0, 1, 2 })
			{
				if (not pair[n].ok)
				{
					return -1;
				}

				HANDLE h = n ? pair[n].read.h : pair[n].write.h;

				constexpr DWORD flag = HANDLE_FLAG_INHERIT;
				if (not SetHandleInformation(h, flag, 0))
				{
					winerr("SetHandleInformation");
					_set_errno(EPERM);
					return -1;
				}
			}

			char cmd[MAX_PATH];
			ZeroMemory(cmd, sizeof cmd);
			for (size_t i = 0, j = 0; argv[i]; ++i, ++j)
			{
				int n = std::snprintf(cmd + j, sizeof cmd - j, "%s ", argv[i]);
				if (0 < n) j += n;
				else
				{
					_set_errno(E2BIG);
					return -1;
				}
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

			constexpr DWORD flag = DETACHED_PROCESS;
			BOOL const ok = CreateProcessA
			(
				nullptr, // application
				cmd,     // command line
				nullptr, // process attributes
				nullptr, // thread attributes
				TRUE,    // inherit handles
				flag,    // creation flags
				nullptr, // environment
				nullptr, // current directory
				&si,     // start-up info
				&pi      // process info
			);

			if (not ok)
			{
				winerr("CreateProcess");
				_set_errno(ECHILD);
				return -1;
			}

			Handle scoped(pi.hThread); // close

			for (int n : { 0, 1, 2 })
			{
				fd[n] = n ? pair[n].read.set(_O_RDONLY) : pair[n].write.set(_O_WRONLY);
			}

			return pi.dwProcessId;
		}
		#else // defined(__POSIX__)
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

			int const res = execvp(argv[0], argv);
			perror("execvp");
			std::exit(res);
		}
		#endif
	}

	pid_t term(pid_t pid)
	{
		#if defined(__WIN32__)
		{
			HANDLE const h = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);
			if (not h)
			{
				sys::winerr("OpenProcess");
			}
			else
			if (not TerminateProcess(h, 0))
			{
				sys::winerr("TerminateProcess");
			}
			return -1;
		}
		#else // defined(__POSIX__)
		{
			if (not fail(pid) and fail(kill(pid, SIGTERM)))
			{
				sys::perror("kill", pid);
			}
			return -1;
		}
		#endif
	}

	void* mem::map(int fd, size_t size, off_t off, int mode, int type)
	{
		#if defined(__WIN32__)
		{
			DWORD protect = 0;
			if (mode & write)
			{
				if (mode & execute)
				{
					protect = PAGE_EXECUTE_READWRITE;
				}
				else
				{
					protect = PAGE_READWRITE;
				}	
			}
			else
			if (mode & execute)
			{
				if (mode & read)
				{
					protect = PAGE_EXECUTE_READ;
				}
				else
				{
					protect = PAGE_EXECUTE;
				}
			}
			else // (mode & read)
			{
				protect = PAGE_READONLY;
			}

			off_t const end = size + off;
			HANDLE const h = CreateFileMapping
			(
				(HANDLE) _get_osfhandle(fd),
				nullptr, // security attributes
				protect,
				HIWORD(end),
				LOWORD(end),
				nullptr // name
			);
			if (not h)
			{
				winerr("CreateFileMapping");
				return nullptr;
			}

			DWORD desired = 0;
			if (mode & write)
			{
				desired |= FILE_MAP_WRITE;
			}
			else // (mode & read)
			{
				desired |= FILE_MAP_READ;
			}
			if (mode & execute)
			{
				desired |= FILE_MAP_EXECUTE;
			}
			if (type & privy)
			{
				desired |= FILE_MAP_COPY;
			}

			auto address = MapViewOfFile
			(
				h,
				desired,
				HIWORD(off),
				LOWORD(off),
				size
			);
			if (address)
			{
				ptr = static_cast<LPVOID>(h);
			}
			else
			{
				sys::winerr("MapViewOfFile");
				Handle scoped(h); // close
			}
			return address;
		}
		#else // defined(__POSIX__)
		{
			int prot = 0;
			if (mode & read) prot |= PROT_READ;
			if (mode & write) prot |= PROT_WRITE;
			if (mode & execute) prot |= PROT_EXEC;

			int flags = 0;
			if (type & share) flags |= MAP_SHARED;
			if (type & privy) flags |= MAP_PRIVATE;
			if (type & fixed) flags |= MAP_FIXED;

			auto address = mmap(nullptr, size, prot, flags, fd, off);
			if (MAP_FAILED == address)
			{
				sys::perror("mmap");
				return nullptr;
			}
			return address;
		}
		#endif
	}

	void mem::unmap(void* address, size_t size)
	{
		#if defined(__WIN32__)
		{
			if (not UnmapViewOfFile(address))
			{
				sys::winerr("UnmapViewOfFile");
			}
			else
			{
				Handle scoped(static_cast<HANDLE>(ptr));
			}
			(void) size;
		}
		#else // defined(__POSIX__)
		{
			if (fail(munmap(address, size)))
			{
				sys::perror("munmap");
			}
		}
		#endif
	}
}

