#ifndef time_hpp
#define time_hpp "POSIX Time"

#include <time.h>

namespace sys::uni::time
{
	struct spec : timespec
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
}

#endif // file