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
		if (sys::win::fail(h)) return -1;
		auto const iptr = reinterpret_cast<intptr_t>(h);
		return _open_osfhandle(iptr, flags);
	}


	template <typename... Args>
	inline void perror(Args... args)
	{
		#ifndef NDEBUG
		{
			auto const s = fmt::error(args...);
			sys::winerr(s.c_str());
		}
		#endif
	}

	template <typename T> struct zero : T
	{
		zero() { ZeroMemory(this, sizeof(T)); }
	};

	template <typename T, DWORD T::*Size> struct sized : zero<T>
	{
		sized() { this->*Size = sizeof(T); }
	};

	struct handle
	{
		HANDLE h;

		handle(HANDLE p = nullptr)
		: h(p)
		{ }

		~handle()
		{
			if (not fail(h) and not CloseHandle(h))
			{
				sys::win::perror("CloseHandle");
			}
		}

		int open(int flags)
		{
			int const fd = sys::win::open(h, flags);
			h = nullptr;
			return fd;
		}

		operator HANDLE() const
		{
			return h;
		}

	};

	using security_attributes = sized<SECURITY_ATTRIBUTES, &SECURITY_ATTRIBUTES::nLength>;
	using startup_info = sized<STARTUPINFO, &STARTUPINFO::cb>;

	class pipe
	{
	public:

		BOOL ok;
		handle read;
		handle write;

		pipe()
		{
			security_attributes sa;
			sa.bInheritHandle = TRUE;
			ok = CreatePipe(&read.h, &write.h, &sa, BUFSIZ);
			if (not ok)
			{
				sys::win::perror("CreatePipe");
			}
		}
	};
}

#endif // file
