#ifndef uni_time_hpp
#define uni_time_hpp "POSIX Time"

#include "sys.hpp"
#include "err.hpp"
#include "ptr.hpp"
#include "err.hpp"
#include <time.h>

namespace sys::uni
{
	struct clock : fwd::zero<std::timespec>
	{
		clock(time_t sec = 0, long nsec = 0)
		{
			tv_sec = sec;
			tv_nsec = nsec;
		}

		bool res(clockid_t id)
		{
			return fail(clock_getres(id, this)) and sys::err(here);
		}

		bool get(clockid_t id)
		{
			return fail(clock_gettime(id, this)) and sys::err(here);
		}

		bool set(clockid_t id) const
		{
			return fail(clock_settime(id, this)) and sys::err(here);
		}
	};

	struct timer : fwd::zero<itimerspec>
	{
		timer(std::timespec interval, std::timespec value)
		{
			it_interval = interval;
			it_value = value;
		}

		bool get(timer_t id)
		{
			return fail(timer_gettime(id, this)) and sys::err(here);
		}

		bool set(timer_t id, int flags = 0, itimerspec* old = nullptr) const
		{
			return fail(timer_settime(id, flags, this, old)) and sys::err(here);
		}
	};
}

#endif // file