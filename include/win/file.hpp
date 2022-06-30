#ifndef win_file_hpp
#define win_file_hpp "WIN32 File API"

#include "win.hpp"
#include "ptr.hpp"
#include "err.hpp"
#include "dir.hpp"
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

	struct find_file : fwd::no_copy, WIN32_FIND_DATA
	{
		HANDLE h;

		find_file(const char *path)
		{
			h = FindFirstFile(path, this);
			if (sys::win::fail(h))
			{
				sys::win::err(here, path);
			}
		}

		~find_file()
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
					sys::win::err(here, "FindNextFile");
				}
				return failure;
			}
			return success;
		}
	};

	struct find_notify
	{
		enum : bool { tree = true, root = false };

		HANDLE h;

		find_notify(const char* path, DWORD dw = 0, bool dir = root)
		{
			h = FindFirstChangeNotification(path, dir, dw);
			if (sys::win::fail(h))
			{
				sys::win::err(here, "FindFirstChangeNotification", path, dir, dw);
			}
		}

		~find_notify()
		{
			if (not FindCloseChangeNotification(h))
			{
				sys::win::err(here, "FindCloseChangeNotification");
			}
		}

		bool next() const
		{
			if (not FindNextChangeNotification(h))
			{
				sys::win::err(here, "FindNextChangeNotification");
				return failure;
			}
			return success;
		}
	};

	struct overlapped : zero<OVERLAPPED>
	{
		using zero::zero;

		overlapped(std::ptrdiff_t off, HANDLE h=invalid)
		{
			hEvent = h;
			large_int large = off;
			Offset = large.low_part();
			OffsetHigh = large.high_part();
		}

		overlapped(void* ptr, HANDLE h=invalid)
		{
			hEvent = h;
			Pointer = ptr;
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

		static auto sub(std::string path)
		{
			return path.append(path.ends_with(sys::sep::dir) ? "*" : "\\*");
		}

	public:

		files(const char *path) : find_file(sub(path).c_str())
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
