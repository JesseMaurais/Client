#include "ipc.hpp"
#include "sys.hpp"
#include "err.hpp"
#include "fmt.hpp"
#include "os.hpp"

#if __has_include(<processthreadapi.h>)
#include <processthreadapi.h>
#endif

namespace sys::io
{
	template
	<
	 class Char,
	 template <class> class Traits,
	 template <class> class Alloc,
	 template <class, class> class basic_stream,
	 sys::file::openmode default_mode
	>
	impl_pstream::impl_pstream()
	{ }

	template
	<
	 class Char,
	 template <class> class Traits,
	 template <class> class Alloc,
	 template <class, class> class basic_stream,
	 sys::file::openmode default_mode
	>
	void basic_pstream::open(initializer_list args, openmode mode)
	{
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

			pipe[STDIN_FILENO] = out.read.release();
			pipe[STDOUT_FILENO] = in.write.release();
		}
		else
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

				std::vector<Char*> cmds(args);
				cmds.push_back(nullptr);

				if (-1 == sys::execv(cmds.front(), cmds.data()))
				{
					sys::perror("execv");
				}
				std::exit(EXIT_FAILURE);
			}
			else
			{
				pipe[STDIN_FILENO] = out.release(STDOUT_FILENO);
				pipe[STDOUT_FILENO] = in.release(STDIN_FILENO);
			}
		}
	}
	
	template class impl_pstream<char>;
	template class impl_pstream<wchar_t>;
}

