#ifndef time_hpp
#define time_hpp "POSIX Time"

#include "ptr.hpp"
#include <time.h>

namespace sys::uni
{
	struct clock : fwd::zero<timespec>
	{
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
		bool get(timer_t id)
		{
			return fail(timer_gettime(id, this)) and err(here);
		}

		bool set(timer_t id, int flags, itimerspec* old = nullptr) const
		{
			return fail(timer_settime(id, flags, this, old)) and err(here);
		}
	};
}

#endif // file