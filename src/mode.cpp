// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "mode.hpp"
#include "file.hpp"
#include "sync.hpp"
#include "sys.hpp"
#include "err.hpp"
#include "type.hpp"
#include "tmp.hpp"
#include <utility>

namespace env::file
{
	fwd::variable<size_t>& width()
	{
		constexpr size_t Size = BUFSIZ;
		static sys::atomic<Size> safe;
		return safe;
	}

	int check(mode am)
	{
		int flags = 0;

		if (am & ok)
		{
			flags |= F_OK;
		}
		if (am & ex)
		{
			flags |= X_OK;
		}
		if (am & rd)
		{
			flags |= R_OK;
		}
		if (am & wr)
		{
			flags |= W_OK;
		}

		return flags;
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

	bool fail(fmt::string::view path, mode am)
	{
		if (not fmt::terminated(path))
		{
			return fail(fmt::to_string(path), am);
		}
		auto const c = path.data();

		#ifdef _WIN32
		if (am & ex)
		{
			DWORD dw;
			return GetBinaryType(c, &dw)
				? success : failure;
		}
		#endif

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

		unsigned const mask = check(am);
		return mask != (state.st_mode & mask);
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
