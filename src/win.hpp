#ifndef win_hpp
#define win_hpp "WIN32 Utility"

#ifndef _WIN32
# error Win32 utility header included without feature macro.
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "type.hpp"
#include "file.hpp"
#include "ptr.hpp"
#include "err.hpp"
#include "str.hpp"
#include "sys.hpp"

namespace sys::win
{
	const char* strerr(DWORD dw, void* h = nullptr);

	template <typename... Args> int err(Args... args)
	{
		if (debug)
		{
			auto const no = GetLastError();
			auto const s = strerr(no);
			return warn(args..., s);
		}
		return -1;
	}

	inline auto invalid = INVALID_HANDLE_VALUE;

	inline bool fail(HANDLE h)
	{
		return nullptr == h or invalid == h;
	}

	inline HANDLE get(int fd)
	{
		if (sys::fail(fd)) return sys::win::invalid;
		auto const iptr = _get_osfhandle(fd);
		return reinterpret_cast<HANDLE>(iptr);
	}

	inline int open(HANDLE h, int flags)
	{
		if (sys::win::fail(h)) return sys::invalid;
		auto const iptr = reinterpret_cast<intptr_t>(h);
		return _open_osfhandle(iptr, flags);
	}

	inline DWORD wait(HANDLE h, DWORD ms = INFINITE)
	{
		auto const dw = WaitForSingleObject(h, ms);
		if (WAIT_FAILED == dw)
		{
			sys::win::err(here, ms);
		}
		return dw;
	}

	inline bool ready(HANDLE h)
	{
		auto const dw = wait(h, 0);
		return WAIT_TIMEOUT == dw;
	}

	template <class T> struct zero : T
	{
		zero() { ZeroMemory(this, sizeof(T)); }
	};

	template <class T, DWORD T::*Size = &T::dwSize> struct size : zero<T>
	{
		size() { this->*Size = sizeof(T); }
	};

	struct security_attributes 
	: size<SECURITY_ATTRIBUTES, &SECURITY_ATTRIBUTES::nLength>
	{
		security_attributes(bool inherit = true)
		{
			bInheritHandle = inherit ? TRUE : FALSE;
		}
	};

	struct info : SYSTEM_INFO
	{
		info() { GetSystemInfo(this); }
	};

	struct handle : fwd::unique
	{
		HANDLE h;

		operator HANDLE() const { return h; }

		handle(HANDLE p = sys::win::invalid) : h(p)
		{ }

		~handle()
		{
			if (not sys::win::fail(h))
			{
				if (not CloseHandle(h))
				{
					sys::win::err(here);
				}
			}
		}

		int open(int flags)
		{
			int const fd = sys::win::open(h, flags);
			h = sys::win::invalid;
			return fd;
		}
	};

	struct pipe
	{
		handle read;
		handle write;

		pipe()
		{
			security_attributes sa;
			if (not CreatePipe(&read.h, &write.h, &sa, BUFSIZ))
			{
				sys::win::err(here);
			}
		} 
	};
}

#endif // file
