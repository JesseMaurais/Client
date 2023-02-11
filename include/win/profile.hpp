#ifndef win_profile_hpp
#define win_profile_hpp "WIN32 Profile"

#include "win.hpp"
#include "ptr.hpp"
#include "tmp.hpp"
#include <cstdlib>
#include <cmath>
#include <ctime>

namespace sys::win
{
	struct large_int : fwd::variable<LONGLONG>
	{
		large_int(LONGLONG value = 0)
		{
			buf->QuadPart = value;
		}

		large_int(DWORD low, LONG high)
		{
			buf->u.LowPart = low;
			buf->u.HighPart = high;
		}

		LONGLONG operator=(LONGLONG value) override
		{
			return buf->QuadPart = value;
		}

		operator LONGLONG() const override
		{
			return buf->QuadPart;
		}

		inline auto low_part() const
		{
			return buf->u.LowPart;
		}

		inline auto high_part() const
		{
			return buf->u.HighPart;
		}

		LARGE_INTEGER buf[1];
	};

	struct counter : large_int
	{
		counter()
		{
			if (not QueryPerformanceCounter(buf))
			{
				#ifdef WINERR
				WINERR("QueryPerformanceCounter");
				#endif
			}
		}
	};

	struct frequency : large_int
	{
		frequency()
		{
			if (not QueryPerformanceFrequency(buf))
			{
				#ifdef WINERR
				WINERR("QueryPerformanceFrequency");
				#endif
			}
		}
	};
}

#endif // file