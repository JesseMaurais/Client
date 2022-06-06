#ifndef dirent_hpp
#define dirent_hpp "POSIX Directory Entry"

#include "err.hpp"
#include "uni.hpp"
#include "ptr.hpp"
#include <dirent.h>

namespace sys::uni
{
	class dir : fwd::no_copy
	{
		DIR *ptr;

	public:

		dir(char const *path)
		{
			ptr = opendir(path);
			if (nullptr == ptr)
			{
				sys::err(here, "opendir", path);
			}
		}

		dir(int fd)
		{
			ptr = fdopendir(fd);
			if (nullptr == ptr)
			{
				sys::err(here, "fdopendir", fd);
			}
		}

		~dir()
		{
			if (nullptr != ptr)
			{
				if (fail(closedir(ptr)))
				{
					sys::err(here, "closedir");
				}
			}
		}

		auto next()
		{
			return ptr ? readdir(ptr) : nullptr;
		}
	};
}

namespace sys
{
	class files : sys::uni::dir
	{
		class iterator
		{
			sys::uni::dir *that;
			struct dirent *ptr;

		public:

			iterator(sys::uni::dir *dir, struct dirent *ent)
			: that(dir), ptr(ent)
			{ }

			bool operator!=(iterator const &it) const
			{
				return it.that != that or it.ptr != ptr;
			}

			auto operator*() const
			{
				return ptr->d_name;
			}

			auto operator->() const
			{
				return ptr;
			}

			auto& operator++()
			{
				ptr = that->next();
				return *this;
			}
		};

	public:

		using dir::dir;

		auto begin()
		{
			return iterator(this, next());
		}

		auto end()
		{
			return iterator(this, nullptr);
		}
	};
}

#endif // file
