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
	void perror(char const *prefix void *module = nullptr);

	template <typename... Args>
	inline void err(Args... args)
	{
		if (sys::debug)
		{
			auto const s = fmt::err(args...);
			sys::win::perror(s.c_str());
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
		if (fail(h)) return sys::invalid;
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
			int const fd = open(h, flags);
			h = INVALID_HANDLE_VALUE;
			return fd;
		}

		void close()
		{
			if (not CloseHandle(h))
			{
				err(here, "CloseHandle");
			}
			else h = INVALID_HANDLE_VALUE;
		}
	};

	struct pipe
	{
		handle read;
		handle write;

		pipe()
		{
			security_attributes sa;
			sa.bInheritHandle = true;
			if (not CreatePipe(&read.h, &write.h, &sa, BUFSIZ))
			{
				err(here, "CreatePipe");
			}
		}
	};

	inline bool fail(pipe const& p)
	{
		return fail(p.read) or fail(p.write);
	}

	struct find : WIN32_FIND_DATA
	{
		HANDLE h;

		find(char const* s)
		{
			h = FindFirstFile(s, this);
			if (fail(h))
			{
				err(here, "FindFirstFile", s);
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
				auto const e = GetLastError();
				if (ERROR_NO_MORE_FILES != e)
				{
					err(here, "FindNextFile");
				}
				return false;
			}
			return true;
		}

		void close()
		{
			if (not FileClose(h))
			{
				err(here, "FindClose");
			}
			else h = INVALID_HANDLE_VALUE;
		}
	};

	inline bool fail(find const& f)
	{
		return fail(f.h);
	}
}

#endif // file
