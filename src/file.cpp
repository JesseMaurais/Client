// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "mode.hpp"
#include "err.hpp"
#include "sys.hpp"
#include "sync.hpp"
#ifdef _WIN32
#include "win/file.hpp"
#else
#include "uni/fctrl.hpp"
#endif

namespace env::file
{
	fwd::variable<size_t>& width()
	{
		static sys::atomic<size_t> safe = BUFSIZ;
		return safe;
	}

	int convert(mode am)
	{
		int flags = 0;

		if (am & rw)
		{
			flags |= O_RDWR;
		}
		else
		if (am & wr)
		{
			flags |= O_WRONLY;
		}
		else
		if (am & rd)
		{
			flags |= O_RDONLY;
		}

		if (am & txt)
		{
			flags |= O_TEXT;
		}
		else
		if (am & bin)
		{
			flags |= O_BINARY;
		}

		if (am & app)
		{
			flags |= O_APPEND;
		}

		if (am & sz)
		{
			flags |= O_TRUNC;
		}

		if (am & xu)
		{
			flags |= O_EXCL;
		}

		if (am & ok)
		{
			flags |= O_CREAT;
		}

		return flags;
	}

	int convert(permit pm)
	{
		int flags = 0;

		if (pm & owner_x)
		{
			flags |= S_IXUSR;
		}
		if (pm & owner_w)
		{
			flags |= S_IWUSR;
		}
		if (pm & owner_r)
		{
			flags |= S_IRUSR;
		}
		if (pm & group_x)
		{
			flags |= S_IXGRP;
		}
		if (pm & group_w)
		{
			flags |= S_IWGRP;
		}
		if (pm & group_r)
		{
			flags |= S_IRGRP;
		}
		if (pm & other_x)
		{
			flags |= S_IXOTH;
		}
		if (pm & other_w)
		{
			flags |= S_IWOTH;
		}
		if (pm & other_r)
		{
			flags |= S_IROTH;
		}

		return flags;
	}

	bool fail(view path, mode mask)
	{
		if (not fmt::terminated(path))
		{
			return fail(fmt::to_string(path), mask);
		}
		auto const c = path.data();

		#ifdef _WIN32
		if (mask & ex)
		{
			DWORD dw;
			return GetBinaryType(c, &dw)
				? success : failure;
		}
		#endif

		if (mask == (mask & rwx))
		{
			int flags = 0;

			if (mask & ok)
			{
				flags |= F_OK;
			}
			if (mask & ex)
			{
				flags |= X_OK;
			}
			if (mask & rd)
			{
				flags |= R_OK;
			}
			if (mask & wr)
			{
				flags |= W_OK;
			}

			return sys::fail(sys::access(c, flags));
		}

		struct sys::stat state(c);
		if (sys::fail(state))
		{
			return failure;
		}

		if ((am & dir) and not S_ISDIR(state.st_mode))
		{
			return failure;
		}
		if ((am & chr) and not S_ISCHR(state.st_mode))
		{
			return failure;
		}
		if ((am & reg) and not S_ISREG(state.st_mode))
		{
			return failure;
		}
		if (am & fifo)
		{
			#ifdef _WIN32
			if (not path.starts_with(R"(\.\pipe\)"))
			#endif
			#ifdef S_ISFIFO
			if (not S_ISFIFO(state.st_mode))
			#endif
				return failure;
		}
		if (am & sock)
		{
			#ifdef S_IFSOCK
			if (not S_ISSOCK(state.st_mode))
			#endif
				return failure;
		}
		if (am & blk)
		{
			#ifdef S_ISFBLK
			if (not S_ISFBLK(state.st_mode)
			#endif
				return failure;
		}
		if (am & reg)
		{
			#ifdef S_ISREG
			if (not S_ISREG(state.st_mode))
			#endif
				return failure;
		}
		if (am & lnk)
		{
			#ifdef S_ISLNK
			if (not S_ISLNK(state.st_mode))
			#endif
				return failure;
		}

		return success;
	}

	unique_ptr make(basic_ptr f)
	{
		return fwd::make_ptr(f, [](auto f)
		{
			if (nullptr != f)
			{
				if (std::fclose(f))
				{
					sys::err(here, "fclose");
				}
			}
		});
	}

	unique_ptr open(string::view path, mode mask)
	{
		if (not fmt::terminated(path))
		{
			const auto s = fmt::to_string(path);
			return open(s, mask);
		}

		if (mask & ex)
		{
			auto f = null_ptr;

			if (mask & wr)
			{
				f = sys::popen(path.data(), "w");
			}
			else
			if (mask & rd)
			{
				f = sys::popen(path.data(), "r");
			}

			if (null_ptr == f)
			{
				sys::err(here, "popen", path);
			}

			return make(f);
		}
	}

	bool lock(basic_ptr f, mode mask, off_t off, size_t sz)
	{
		const auto fd = sys::fileno(f);
		#ifdef alert
		alert(fail(fd));
		#endif

		#ifdef _WIN32
		{
			DWORD dw = 0;
			if (not (mask & wr))
			{
				dw |= LOCKFILE_EXCLUSIVE_LOCK;
			}
			if (mask & ok)
			{
				dw |= LOCKFILE_FAIL_IMMEDIATELY
			}
			if (0 == sz)
			{
				sz = MAXDWORD;
			}

			const auto h = sys::win::get(fd);
			if (mask & un)
			{
				if (UnlockFile(h, LOWORD(off), HIWORD(off), LOWORD(sz), HIWORD(sz)))
				{
					return success;
				}
			}
			else
			{
				fwd::zero<OVERLAPPED> over;
				over.DUMMYUNIONNAME.DUMMYSTRUCTNAME.Offset = LOWORD(off);
				over.DUMMYUNIONNAME.DUMMYSTRUCTNAME.OffsetHigh = HIWORD(off);
				if (LockFileEx(h, dw, 0, LOWORD(sz), HIWORD(sz), &over))
				{
					return success;
				}
			}
			sys::win::err(here);
			return failure;
		}
		#else // UNIX
		{
			sys::uni::file::lock key;
			key.l_whence = SEEK_SET;
			key.l_start = off;
			key.l_len = sz;

			if (mask & un)
			{
				key.l_type = F_UNLCK;
			}
			else
			if (mask & wr)
			{
				key.l_type = F_WRLCK;
			}
			else
			if (mask & rd)
			{
				key.l_type = F_RDLCK;
			}
			if (mask & ok)
			{
				return key.set(fd);
			}
			else
			{
				return key.wait(fd);
			}
		}
		#endif
	}
}

#ifdef test_unit
#include "arg.hpp"
test_unit(mode)
{
	assert(not env::file::fail(__FILE__) and "Source file exists");
	assert(not env::file::fail(env::opt::arg(), env::file::ex) and "Program is executable");
}
#endif