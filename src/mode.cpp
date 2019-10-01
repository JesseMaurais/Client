// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "file.hpp"
#include "thread.hpp"
#include "sys.hpp"
#include "err.hpp"
#include "fmt.hpp"
#include <utility>

namespace env::file
{
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

	size_t width(ssize_t n)
	{
		static sys::rwlock lock;
		static ssize_t sz = BUFSIZ;
		if (n < 0)
		{
			auto unlock = lock.read();
			return sz;
		}
		auto unlock = lock.write();
		std::swap(n, sz);
		return n;
	}

	bool fail(fmt::view path, mode am)
	{
		if (not fmt::terminated(path))
		{
			auto const s = fmt::to_string(path);
			return fail(s, am);
		}

		return fail(sys::access(data(path), check(am)));
	}
}

