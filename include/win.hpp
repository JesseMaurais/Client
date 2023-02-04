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
#include "sys.hpp"

#ifndef WINERR
# define WINERR(X, ...) ::sys::win::err(HERE(X), __VA_ARGS__)
#endif

namespace sys::win
{
	const char* strerr(DWORD dw, void* h = nullptr);

	template <typename... Args> auto& err(fmt::where at, Args... args)
	{
		auto const no = GetLastError();
		auto const s = strerr(no);
		return fmt::warn(at, args..., s);
	}

	inline auto invalid = INVALID_HANDLE_VALUE;

	inline bool fail(HANDLE h)
	{
		return nullptr == h or invalid == h;
	}

	using handless = fwd::no_ptr<HANDLE>;

	inline auto enclose(HANDLE h)
	{
		return fwd::make_shared<handless>(h, [](auto h)
		{
			if (not fail(h) and not CloseHandle(h))
			{
				WINERR("CloseHandle");
			}
		});
	}

	struct handle : fwd::shared_ptr<handless>
	{
		using basic = fwd::shared_ptr<handless>;

		handle(HANDLE h = nullptr)
		: basic(h, enclose)
		{ }

		operator HANDLE() const
		{
			return get();
		}
	};

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
		auto dw = WaitForSingleObject(h, ms);
		if (WAIT_FAILED == dw)
		{
			WINERR("WaitForSingleObject", ms);
		}
		return dw;
	}

	inline DWORD wait(std::span<HANDLE> h, BOOL a = FALSE, DWORD ms = INFINITE)
	{
		auto sz = h.size();
		auto dw = WaitForMultipleObjects(static_cast<DWORD>(sz), h.data(), a, ms);
		if (WAIT_FAILED == dw)
		{
			WINERR("WaitForMultipleObjects", ms);
		}
		return dw;
	}

	inline bool got(HANDLE h)
	{
		return WAIT_TIMEOUT != wait(h, 0);
	}

	template <class T> struct zero : T
	{
		static_assert(std::is_trivially_copyable<T>::value);
		static_assert(std::is_standard_layout<T>::value);
		static constexpr auto sizeof_zero = sizeof(T);

		zero()
		{
			ZeroMemory(this, sizeof_zero);
		}

		operator T*() const
		{
			return this;
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
	
	struct wndclass : size<&WNDCLASSEX::cbSize>
	{
		auto set() const
		{
			return RegisterClassEx(this);
		}
		
		auto get(LPSTR s, HINSTANCE h = nullptr)
		{
			return GetClassInfoEx(h, s, this);
		}
	};
		
	struct info : SYSTEM_INFO
	{
		info() { GetSystemInfo(this); }
	};

	struct pipe
	{
		handle read;
		handle write;

		pipe(LPSECURITY_ATTRIBUTES lp = nullptr, DWORD sz = 0)
		{
			if (HANDLE h[2]; CreatePipe(h + 0, h + 1, lp, sz))
			{
				read.reset(h + 0), write.reset(h + 1);
			}
			else
			{
				#ifdef WINERR
				WINERR("CreatePipe");
				#endif
			}
		}
	};
}

#endif // file
