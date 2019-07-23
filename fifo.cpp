// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "fifo.hpp"
#include "sys.hpp"
#include "env.hpp"
#include "fmt.hpp"
#include "err.hpp"
#include "dir.hpp"

#ifdef _WIN32
# include "win.hpp"
#endif

namespace sys::file
{
	fifo::fifo(fmt::string_view name, mode mask)
	: flags(convert(mask))
	{
		#ifdef _WIN32
		{
			path = fmt::join("\\\\.\\pipe\\", name);

			sys::handle const h = CreateNamedPipe
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

			if (sys::win::fail(h))
			{
				sys::win::perror("CreateNamedPipe", name);
				path.clear();
				return;
			}

			fd = h.open(flags);
		}
		#else
		{
			auto const dir = fmt::dir::join(::env::dir::run, ".fifo");
			auto const s = dir.c_str();

			constexpr mode_t rw = S_IRGRP | S_IWGRP;
			sys::mode const um;

			if (sys::fail(sys::access(s, F_OK)))
			{
				if (sys::fail(sys::mkdir(s, um | rw)))
				{
					sys::perror("mkdir", dir);
					return;
				}
			}

			if (sys::fail(sys::access(s, R_OK | W_OK)))
			{
				if (sys::fail(sys::chmod(s, um | rw)))
				{
					sys::perror("chmod", dir);
					return;
				}
			}

			path = fmt::dir::join(dir, name);
			auto const ps = path.c_str();
			if (sys::fail(mkfifo(ps, um | rw)))
			{
				sys::perror("mkfifo", path);
				path.clear();
			}
		}
		#endif
	}

	bool fifo::connect()
	{
		#ifdef _WIN32
		{
			auto const h = sys::win::get(fd);
			bool const ok = not sys::win::fail(h);
			if (ok and not ConnectNamedPipe(h, nullptr))
			{
				sys::win::perror("ConnectNamedPipe");
				path.clear();
				return false;
			}
			return ok;
		}
		#else
		{
			sys::mode const um;
			auto const s = path.c_str();
			fd = sys::open(s, flags, (mode_t) um);
			if (sys::fail(fd))
			{
				sys::perror("open", path);
				path.clear();
				return false;
			}
			return true;
		}
		#endif
	}

	fifo::~fifo()
	{
		#ifdef _WIN32
		{
			auto const h = sys::win::get(fd);
			if (not sys::win::fail(h) and not DisconnectNamedPipe(h))
			{
				sys::win::perror("DisconnectNamedPipe");
			}
		}
		#else
		{
			if (not empty(path))
			{
				auto const s = path.c_str();
				if (sys::unlink(s))
				{
					sys::perror("unlink", path);
				}
			}
		}
		#endif
	}
}
