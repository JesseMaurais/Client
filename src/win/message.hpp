#ifndef win_msg_hpp
#define win_msg_hpp

#include "win.hpp"
#include "sys.hpp"
#include "err.hpp"
#include "type.hpp"

namespace sys::win
{
	struct msg : MSG
	{
		template <UINT Min, UINT Max> struct range : ::fmt::range<UINT>
		{
			using base = ::fmt::range<UINT>;
			range() : base(Min, Max) { }
		};

		using key   = range<WM_KEYFIRST, WM_KEYLAST>;
		using mouse = range<WM_MOUSEFIRST, WM_MOUSELAST>;
		using user  = range<WM_USER, 0x7FFF>;
		using app   = range<WM_APP, 0xBFFF>;
		using text  = range<0xC000, 0xFFFF>;

		inline bool get(UINT min = 0, UINT max = 0, HWND hw = nullptr)
		{
			auto const result = GetMessage(this, hw, min, max);
			if (sys::fail(result))
			{
				sys::win::err(here, "GetMessage");
				return failure;
			}
			return success;
		}

		static bool put(DWORD the, UINT msg, WPARAM wp = 0, LPARAM lp = 0)
		{
			if (not PostThreadMessage(the, msg, wp, lp))
			{
				sys::win::err
				(
					here, "PostThreadMessage", the, msg, wp, lp
				);
				return failure;
			}
			return success;
		}
	};
}

#endif // file
