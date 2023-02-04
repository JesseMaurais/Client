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
				#ifdef WINERR
				WINERR("GetMessage");
				#endif
				return failure;
			}
			return success;
		}

		static bool put(DWORD dw, UINT msg, WPARAM wp = 0, LPARAM lp = 0)
		{
			if (not PostThreadMessage(dw, msg, wp, lp))
			{
				#ifdef WINERR
				WINERR("PostThreadMessage");
				#endif
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
}

#endif // file
