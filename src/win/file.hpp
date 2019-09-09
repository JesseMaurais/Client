#ifndef win_dir_hpp
#define win_dir_hpp

#include "win.hpp"
#include "ptr.hpp"
#include "err.hpp"
#include <cstring>
#include <string>

namespace sys::win
{
	struct file_info : BY_HANDLE_FILE_INFORMATION
	{
		file_info(HANDLE h)
		{
			if (not GetFileInformationByHandle(h, this))
			{
				sys::win::err(here);
			}
		}
	};

	struct find_file : unique, WIN32_FIND_DATA
	{
		HANDLE h;

		find_files(char const *path)
		{
			h = FindFirstFile(path, this);
			if (sys::win::fail(h))
			{
				sys::win::err(here, path);
			}
		}

		~find_files()
		{
			if (not sys::win::fail(h))
			{
				if (not FindClose(h))
				{
					sys::win::err(here);
				}
			}
		}

		bool next()
		{
			if (not FindNextFile(h, this))
			{
				if (GetLastError() != ERROR_NO_MORE_FILES)
				{
					sys::win::err(here);
				}
				return failure;
			}
			return success;
		}
	};
}

namespace sys
{
	class files : sys::win::find_file
	{
		class iterator
		{
			sys::win::find_file *that;
			bool flag; 

		public:

			iterator(sys::win::find_file* ptr, bool end) 
			: that(ptr), flag(end)
			{ }

			bool operator!=(iterator const &it) const
			{
				return it.that != that or it.flag != flag;
			}

			auto operator*() const
			{
				auto const c = std::strrchr(that->cFileName, '\\');
				return nullptr == c ? that->cFileName : c + 1;
			}

			auto operator->() const
			{
				return (LPWIN32_FIND_DATA) that;
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

		files(char const *path) : find_file(data(sub(path)))
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
