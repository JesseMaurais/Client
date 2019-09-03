#ifndef uni_files_hpp
#define uni_files_hpp

#include <dirent.h>
#include "uni.hpp"
#include "ptr.hpp"
#include "err.hpp"

namespace sys::uni
{
	class directory : unique
	{
		DIR *ptr;

	public:

		directory(char const *path)
		{
			ptr = opendir(path);
			if (nullptr == ptr)
			{
				sys::err(here, path);
			}
		}

		~directory()
		{
			if (nullptr != ptr)
			{
				if (sys::fail(closedir(ptr)))
				{
					sys::err(here);
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
	class files : sys::uni::directory
	{
		class iterator
		{
			sys::uni::directory *that;
			struct dirent *ptr;

		public:

			iterator(sys::uni::directory *dir, struct dirent *ent)
			: that(dir), ptr(ent)
			{ }

			struct dirent const *operator->() const
			{
				return ptr;
			}

			bool operator!=(iterator const &it) const
			{
				return it.that != that or it.ptr != ptr;
			}

			auto operator*() const
			{
				return ptr->d_name;
			}

			auto& operator++()
			{
				ptr = that->next();
				return *this;
			}
		};

	public:

		using directory::directory;

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
