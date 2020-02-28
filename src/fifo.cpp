// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "fifo.hpp"
#include "file.hpp"
#include "sys.hpp"
#include "opt.hpp"
#include "fmt.hpp"
#include "err.hpp"
#include "dir.hpp"
#include "pos.hpp"

namespace env::file
{
	fifo::fifo(fmt::string_view name, mode mask)
	: flags(convert(mask))
	{
		#ifdef _WIN32
		{
			path = fmt::join({ "\\\\.\\pipe\\", name });

			auto const size = width();
			sys::win::handle h = CreateNamedPipe
			(
				path.c_str(),
				PIPE_ACCESS_DUPLEX,
				PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
				PIPE_UNLIMITED_INSTANCES,
				size,   // output
				size,   // input
				0,      // default time out
				nullptr // default security
			);

			if (sys::win::fail(h))
			{
				sys::win::err(here, "CreateNamedPipe", path);
				path.clear();
				return;
			}

			fd = h.open(flags);
		}
		#else
		{
			auto const dir = fmt::dir::join({env::opt::run, ".fifo"});
			auto const s = dir.data();

			constexpr mode_t rw = S_IRGRP | S_IWGRP;
			sys::mode const um;

			if (fail(sys::access(s, F_OK)))
			{
				if (fail(sys::mkdir(s, um | rw)))
				{
					sys::err(here, "mkdir", dir);
					return;
				}
			}

			if (fail(sys::access(s, R_OK | W_OK)))
			{
				if (fail(sys::chmod(s, um | rw)))
				{
					sys::err(here, "chmod", dir);
					return;
				}
			}

			path = fmt::dir::join({dir, name});
			auto const c = path.data();
			if (fail(mkfifo(c, um | rw)))
			{
				sys::err(here, "mkfifo", path);
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
				sys::win::err(here, "ConnectNamedPipe");
				path.clear();
				return false;
			}
			return ok;
		}
		#else
		{
			sys::mode const um;
			auto const s = data(path);
			fd = sys::open(s, flags, (mode_t) um);
			if (fail(fd))
			{
				sys::err(here, "open", path);
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
			bool const ok = not sys::win::fail(h);
			if (ok and not DisconnectNamedPipe(h))
			{
				sys::win::err(here, "DisconnectNamedPipe");
			}
		}
		#else
		{
			if (not empty(path))
			{
				auto const c = path.data();
				if (fail(sys::unlink(c)))
				{
					sys::err(here, "unlink", path);
				}
			}
		}
		#endif
	}
}
