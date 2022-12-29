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
			auto e = clock_getres(id, this);
			if (fail(e)) perror("clock_getres");
			return e;
		}

		bool get(clockid_t id)
		{
			auto e = clock_gettime(id, this);
			if (fail(e)) perror("clock_gettime");
			return e;
		}

		bool set(clockid_t id) const
		{
			auto e = clock_settime(id, this);
			if (fail(e)) perror("clock_settime");
			return e;
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
			auto e = timer_gettime(id, this);
			if (fail(e)) perror("timer_gettime");
			return e;
		}

		bool set(timer_t id, int flags = 0, itimerspec* old = nullptr) const
		{
			auto e = timer_settime(id, flags, this, old);
			if (fail(e)) perror("timer_settime");
			return e;
		}
	};
}

#endif // file
