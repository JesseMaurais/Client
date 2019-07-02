// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "fifo.hpp"
#include "sys.hpp"
#include "env.hpp"
#include "fmt.hpp"
#include "err.hpp"

#if __has_include(<windows.h>)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

using namespace fmt;

namespace sys::file
{
	fifo::fifo(string_view name, openmode mode)
	{
		int const flags = convert(mode);

		#if defined(__WIN32__)
		{
			constexpr auto prefix = "\\\\.\\pipe";
			path = join({ prefix, name }, "\\");

			sys::handle h = CreateNamedPipe
			(
				path.c_str(),
				PIPE_ACCESS_DUPLEX,
				PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
				PIPE_UNLIMITED_INSTANCES,
				BUFSIZ, // output
				BUFSIZ, // input
				0,      // default time out
				nullptr // default security
			);

			if (nullptr == h)
			{
				sys::winerr("CreateNamedPipe");
				path.clear();
				return;
			}

			if (not ConnectNamedPipe(h, nullptr))
			{
				sys::winerr("ConnectNamedPipe");
				path.clear();
				return;
			}

			fd = h.open(flags);
		}
		#endif

		#if defined(__POSIX__)
		{
			constexpr auto mask = 0777;
			static auto const dir = join({ ::env::tmpdir, ".pipe" }, sys::sep::dir);

			int const exists = sys::access(dir.c_str(), F_OK);
			if (fail(exists) and fail(sys::mkdir(dir.c_str(), mask)))
			{
				sys::perror("mkdir");
				return;
			}

			path = join({ dir, name }, sys::sep::dir);
			if (mkfifo(path.c_str(), mask))
			{
				sys::perror("mkfifo", path);
				path.clear();
				return;
			}

			fd = sys::open(path.c_str(), flags, mask);
			if (fail(fd))
			{
				sys::perror("open", path);
				path.clear();
				return;
			}
		}
		#endif
	}

	fifo::~fifo()
	{
		#if defined(__WIN32__)
		{
			auto const ptr = _get_osfhandle(fd);
			auto const h = reinterpret_cast<HANDLE>(ptr);

			if (h and not DisconnectNamedPipe(h))
			{
				sys::winerr("DisconnectNamedPipe");
			}
		}
		#endif

		#if defined(__POSIX__)
		{
			if (not empty(path))
			{
				if (sys::unlink(path.c_str()))
				{
					sys::perror("unlink", path);
				}
			}
		}
		#endif
	}
}
