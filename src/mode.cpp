// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "mode.hpp"
#include "file.hpp"
#include "sys.hpp"
#include "err.hpp"
#include "fmt.hpp"
#include "tmp.hpp"
#include <utility>
#ifdef _WIN32
#include "win/sync.hpp"
#else
#include "uni/pthread.hpp"
#endif

namespace
{
	struct : attribute<size_t>
	{
		mutable sys::rwlock lock;
		size_t sz = BUFSIZ;

		operator size_t() const final
		{
			auto const unlock = lock.read();
			return sz;
		}

		size_t operator=(size_t n) final
		{
			auto const unlock = lock.write();
			return sz = n;
		}

	} width;
}

namespace env::file
{
	attribute<size_t>& width = ::width;

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

	bool fail(fmt::string_view path, mode am)
	{
		if (not fmt::terminated(path))
		{
			auto const s = fmt::to_string(path);
			return fail(s, am);
		}

		return fail(sys::access(data(path), check(am)));
	}
}
