#ifndef time_hpp
#define time_hpp "POSIX Time"

#include "ptr.hpp"
#include "err.hpp"
#include <time.h>

namespace sys::uni
{
	struct clock : fwd::zero<timespec>
	{
		clock(time_t sec = 0, long nsec = 0)
		{
			tv_sec = sec;
			tv_nsec = nsec;
		}

		bool res(clockid_t id)
		{
			return fail(clock_getres(id, this)) and err(here);
		}

		bool get(clockid_t id)
		{
			return fail(clock_gettime(id, this)) and err(here);
		}

		bool set(clockid_t id) const
		{
			return fail(clock_settime(id, this)) and err(here);
		}
	};

	struct timer : fwd::zero<itimerspec>
	{
		timer(clock interval, clock value)
		{
			it_interval = interval;
			it_value = value;
		}

		bool get(timer_t id)
		{
			return fail(timer_gettime(id, this)) and err(here);
		}

		bool set(timer_t id, int flags, itimerspec* old = nullptr) const
		{
			return fail(timer_settime(id, flags, this, old)) and err(here);
		}
	};

	constexpr long nsecpersec = 1e9;
}

static timerspec operator+(const timerspec& left, const timerspec& right)
{
	const auto sec = left.tv_sec + right.tv_sec;
	const auto nsec = left.tv_nsec + right.tv_nsec;
	const auto div = std::div(nsec, sys::uni::nsecpersec);
	return { .tv_sec = sec + div.quot, .tv_nsec = div.rem };
}

static timerspec operator-(const timerspec& first, const timerspec& second)
{
	auto sec = left.tv_sec - right.tv_sec;
	auto nsec = left.tv_nsec - right.tv_nsec;
	while (nec < 0) --sec, nsec += sys::uni::nsecpersec;
	return { .tv_sec = sec, .tv_nsec = nsec };
}

#endif // file