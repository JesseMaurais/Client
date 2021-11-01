#ifndef win_profile_hpp
#define win_profile_hpp "WIN32 Profile"

#include "win.hpp"
#include "ptr.hpp"
#include "tmp.hpp"
#include <cstdlib>

namespace sys::win
{
	struct large_int : fwd::variable<long long>
	{
		long long operator=(long long value) override
		{
			return buf->QuadPart = value;
		}

		operator long long() const override
		{
			return buf->QuadPart;
		}

	protected:

		LARGE_INTEGER buf[1];
	};

	struct counter : large_int
	{
		counter()
		{
			if (not QueryPerformanceCounter(buf))
			{
				sys::win::err(here, "QueryPerformanceCounter");
			}
		}
	};

	struct frequency : large_int
	{
		frequency()
		{
			if (not QueryPerformanceFrequency(buf))
			{
				sys::win::err(here, "QueryPerformanceFrequency");
			}
		}
	};

	static auto ticks(long long period)
	{
		const auto counter t;
		static const auto frequency hz;
		return std::lldiv(t, hz);
	}
}

#endif // file