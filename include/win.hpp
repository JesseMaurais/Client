#ifndef win_hpp
#define win_hpp "WIN32 Utility"

#ifndef _WIN32
# error WIN32 utility header included without feature macro.
#endif

#include <windows.h>
#include "type.hpp"
#include "file.hpp"
#include "mode.hpp"
#include "ptr.hpp"
#include "err.hpp"
#include "tag.hpp"
#include "sys.hpp"

namespace sys::win
{
	const char* strerr(DWORD dw, void* h = nullptr);

	template <typename... Args> int err(fmt::where at, Args... args)
	{
		if (debug)
		{
			auto const no = GetLastError();
			auto const s = strerr(no);
			return warn(at, args..., s);
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
		return WAIT_TIMEOUT == wait(h, 0);
	}

	template <class T> struct zero : T
	{
		static_assert(std::is_trivially_copyable<T>::value);
		static_assert(std::is_standard_layout<T>::value);

		zero()
		{
			ZeroMemory(this, sizeof(T));
		}
	};

	template
	<
		auto S, class T = fwd::offset_of<S>
	>
	struct size : zero<typename T::parent_type>
	{
		size()
		{
			constexpr auto s = sizeof(typename T::parent_type);
			this->*S = static_cast<typename T::value_type>(s);
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
					sys::win::err(here, "CloseHandle");
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

		pipe(LPSECURITY_ATTRIBUTES lp = nullptr)
		{
			env::file::size_t const sz = env::file::width();
			if (not CreatePipe(&read.h, &write.h, lp, sz))
			{
				sys::win::err(here, "CreatePipe");
			}
		}
	};
}

#endif // file
