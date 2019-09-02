#ifndef win_dir_hpp
#define win_dir_hpp

#include "win.hpp"
#include "ptr.hpp"
#include "err.hpp"
#include "str.hpp"

namespace sys::win
{
	struct find_files : unique, WIN32_FIND_DATA
	{
		HANDLE h = invalid;

		find_files(char const *path)
		{
			if (nullptr != path)
			{
				h = FindFirstFile(path, this);
				if (sys::win::fail(h))
				{
					sys::win::err(here, "FindFirstFile", path);
				}
			}
		}

		~find_files()
		{
			if (not sys::win::fail(h))
			{
				if (not FindClose(h))
				{
					sys::win::err(here, "FindClose");
				}
			}
		}

		bool next()
		{
			if (not FindNextFile(h, this))
			{
				if (GetLastError() != ERROR_NO_MORE_FILES)
				{
					sys::win::err(here, "FindNextFile");
				}
				return failure;
			}
			return success;
		}
	};
}

namespace sys
{
	class files : sys::win::find_files
	{
		class iterator
		{
			sys::win::find_files *that;
			bool flag; 

		public:

			iterator(sys::win::find_files* ptr, bool end) 
			: that(ptr), flag(end)
			{ }

			auto operator*() const
			{
				return that->cFileName;
			}

			auto& operator++()
			{
				flag = that->next();
				return *this;
			}
		};

		static auto sub(char const *path)
		{
			return std::string(path) + "\\*";
		}

	public:

		files(char const *path) : find_files(data(sub(path)))
		{ }

		auto begin()
		{
			return iterator(this, sys::win::fail(h));
		}

		auto end()
		{
			return iterator(this, true);
		}
	};
}

#endif // file
