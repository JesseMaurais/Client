#ifndef win_hpp
#define win_hpp

#ifndef _WIN32
# error Win32 utility header included without feature macro.
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "sys.hpp"
#include "err.hpp"

namespace sys::win
{
	void error(char const *prefix void *module = nullptr);

	template <typename... Args>
	inline void perror(Args... args)
	{
		if (sys::debug)
		{
			auto const s = fmt::error(args...);
			error(s.c_str());
		}
	}

	inline bool fail(HANDLE h)
	{
		return nullptr == h or INVALID_HANDLE_VALUE == h;
	}

	inline HANDLE get(int fd)
	{
		if (sys::fail(fd)) return INVALID_HANDLE_VALUE;
		auto const iptr = _get_osfhandle(fd);
		return reinterpret_cast<HANDLE>(iptr);
	}

	inline int open(HANDLE h, int flags)
	{
		if (sys::win::fail(h)) return sys::invalid;
		auto const iptr = reinterpret_cast<intptr_t>(h);
		return _open_osfhandle(iptr, flags);
	}
	template <typename T> struct zero : T
	{
		zero() { ZeroMemory(this, sizeof(T)); }
	};

	template <typename T, DWORD T::*Size> struct size : zero<T>
	{
		size() { this->*Size = sizeof(T); }
	};

	using security_attributes = size<SECURITY_ATTRIBUTES, &SECURITY_ATTRIBUTES::nLength>;
	using startup_info = size<STARTUPINFO, &STARTUPINFO::cb>;
	using process_info = zero<PROCESS_INFORMATION>;

	struct handle
	{
		HANDLE h;

		handle(HANDLE p = nullptr)
		: h(p)
		{ }

		~handle()
		{
			if (not fail(h))
			{
				close();
			}
		}

		int open(int flags)
		{
			int const fd = sys::win::open(h, flags);
			h = INVALID_HANDLE_VALUE;
			return fd;
		}

		void close()
		{
			if (not CloseHandle(h))
			{
				sys::win::perror(here, "CloseHandle");
			}
			h = nullptr;
		}

		operator HANDLE() const
		{
			return h;
		}

	};

	struct pipe
	{
		BOOL ok;
		handle read;
		handle write;

		pipe()
		{
			security_attributes sa;
			sa.bInheritHandle = true;
			ok = CreatePipe(&read.h, &write.h, &sa, BUFSIZ);
			if (not ok)
			{
				sys::win::perror(here, "CreatePipe");
			}
		}
	};

	struct find : WIN32_FIND_DATA
	{
		HANDLE h;

		operator bool() const
		{
			return not fail(h);
		}

		void close()
		{
			if (not FileClose(h))
			{
				sys::win::perror(here, "FindClose");
			}
			h = nullptr;
		}

		find(char const* s)
		{
			h = FindFirstFile(s, this);
			if (fail(h))
			{
				sys::win::perror(here, "FindFirstFile", s);
			}
		}

		~find()
		{
			if (not fail(h))
			{
				close();
			}
		}

		bool operator++()
		{
			if (not FindNextFile(h, this))
			{
				auto const err = GetLastError();
				if (ERROR_NO_MORE_FILES != err)
				{
					sys::win::perror(here, "FindNextFile");
				}
				return false;
			}
			return true;
		}
	};
}

#endif // file
