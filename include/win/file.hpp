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
				#ifdef WINERR
				WINERR("GetFileInformationByHandle");
				#endif
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
				#ifdef WINERR
				WINERR("FindFirstFile", path);
				#endif
			}
		}

		~find_file()
		{
			if (not sys::win::fail(h))
			{
				if (not FindClose(h))
				{
					#ifdef WINERR
					WINERR("FindClose");
					#endif
				}
			}
		}

		bool next()
		{
			if (not FindNextFile(h, this))
			{
				if (GetLastError() != ERROR_NO_MORE_FILES)
				{
					#ifdef WINERR
					WINERR("FindNextFile");
					#endif
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

		operator HANDLE() const
		{
			return h;
		}

		find_notify(const char* path, DWORD dw = 0, bool dir = root)
		{
			h = FindFirstChangeNotification(path, dir, dw);
			if (sys::win::fail(h))
			{
				#ifdef WINERR
				WINERR("FindFirstChangeNotification", path, dir, dw);
				#endif
			}
		}

		~find_notify()
		{
			if (not FindCloseChangeNotification(h))
			{
				#ifdef WINERR
				WINERR("FindCloseChangeNotification");
				#endif
			}
		}

		bool next() const
		{
			if (not FindNextChangeNotification(h))
			{
				#ifdef WINERR
				WINERR("FindNextChangeNotification");
				#endif
				return failure;
			}
			return success;
		}
	};

	struct notify_info : zero<FILE_NOTIFY_INFORMATION>
	{
		WCHAR buf[FILENAME_MAX];

		bool read(HANDLE h, BOOL sub, DWORD dw, LPOVERLAPPED lp=nullptr, LPOVERLAPPED_COMPLETION_ROUTINE fn=nullptr)
		{
			constexpr auto sz = sizeof(FILE_NOTIFY_INFORMATION) + sizeof(WCHAR) * FILENAME_MAX;
			if (not ReadDirectoryChangesW(h, this, sz, sub, dw, nullptr, lp, fn))
			{
				#ifdef WINERR
				WINERR("ReadDirectoryChangesW");
				#endif
				return failure;
			}
			return success;
		}

		fmt::wide value() const
		{
			return { FileName, FileNameLength / sizeof(WCHAR) };
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

		overlapped(HANDLE h=invalid, auto ptr=nullptr)
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
			return path.append(path.ends_with(sys::tag::dir) ? "*" : "\\*");
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
