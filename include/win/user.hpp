#ifndef win_user_hpp
#define win_user_hpp "WIN32 User"

#include "win.hpp"
#include "sys.hpp"
#include "type.hpp"
#include "err.hpp"
#include "it.hpp"

namespace sys::win
{
	struct msg : MSG
	{
		using key   = fwd::interval<WM_KEYFIRST, WM_KEYLAST>;
		using mouse = fwd::interval<WM_MOUSEFIRST, WM_MOUSELAST>;
		using user  = fwd::interval<WM_USER, 0x7FFF>;
		using app   = fwd::interval<WM_APP, 0xBFFF>;
		using text  = fwd::interval<0xC000, 0xFFFF>;

		bool get(UINT min = 0, UINT max = 0, HWND hw = nullptr)
		{
			auto const result = GetMessage(this, hw, min, max);
			if (sys::fail(result))
			{
				sys::win::err(here, "GetMessage");
				return failure;
			}
			return success;
		}

		static bool put(DWORD dw, UINT msg, WPARAM wp = 0, LPARAM lp = 0)
		{
			if (not PostThreadMessage(dw, msg, wp, lp))
			{
				sys::win::err
				(
					here, "PostThreadMessage", dw, msg, wp, lp
				);
				return failure;
			}
			return success;
		}
	};

	struct classinfo : size<&WNDCLASSEX::cbSize>
	{
		classinfo() = default;

		bool get(LPCSTR c, HINSTANCE h = nullptr)
		{
			return GetClassInfoEx(h, c, this);
		}

		auto set() const
		{
			return RegisterClassEx(this);
		}
	};

	struct timer
	{
		using function = std::function<TIMERPROC>

		timer(funciton f, UINT t, HWND h = nullptr)
		{
			id = SetTimer(h, static_cast<UINT_PTR>(this), t, thread);
			if (not id) sys::win::err(here, "SetTimer", h, t);
			else work = f;
		}

		void kill(HWND h = nullptr)
		{
			if (id)
			{
				if (not KillTimer(h, id))
				{
					sys::win::err(here, "KillTimer", h, id);
				}
			}
		}

	private:

		function work;

		static void thread(HWND h, UINT u, UINT_PTR ptr, DWORD dw)
		{
			auto that = static_cast<fwd::as_ptr<timer>>(ptr);
			#ifdef assert
			assert(nullptr != that);
			#endif
			if (that) that->work(h, u, dw);
		}
	};
}

#endif // file
