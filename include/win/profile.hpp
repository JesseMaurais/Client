#ifndef win_profile_hpp
#define win_profile_hpp "WIN32 Profile"

#include "win.hpp"
#include "ptr.hpp"
#include "tmp.hpp"
#include <cstdlib>
#include <cmath>

namespace sys::win
{
	struct large_int : fwd::variable<long long>
	{
		large_int(long long value)
		{
			buf->QuadPart = value;
		}

		long long operator=(long long value) override
		{
			return buf->QuadPart = value;
		}

		operator long long() const override
		{
			return buf->QuadPart;
		}

		inline auto low_part() const
		{
			return DUMMYSTRUCTNAME.LowPart;
		}

		inline auto high_part() const
		{
			return DUMMYSTRUCTNAME.HighPart;
		}

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

	static auto div(long long persec)
	{
		static const frequency hz;
		return std::lldiv(persec, hz);
	}

	static auto fma(long long persec, long long n)
	{
		const auto f = div(persec);
		return std::fma(n, f.quot, f.rem);
	}

	struct timer : handle
	{
		timer(HANDLE h, fwd::function f, long long t, long p = 0, bool resume = true) : work(f), handle(h)
		{
			const large_int large = t;
			if (not SetWaitableTimer(h, large.buf, p, thread, this, resume))
			{
				sys::win::err(here, "SetWaitableTimer");
			}
		}

		~timer()
		{
			if (not sys::win::fail(h))
			{
				if (not CancelWaitableTimer(h))
				{
					sys::win::err(here, "CancelWaitableTimer");
				}
			}
		}

	private:

		function work;

		static void CALLBACK thread(LPVOID lp, DWORD low, DWORD high)
		{
			auto that = fwd::cast_as<timer>(lp);
			#ifdef assert
			assert(nullptr != that);
			#endif
			if (that) that->work();
		}
	};
}

#endif // file