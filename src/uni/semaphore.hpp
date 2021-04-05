#ifndef uni_semaphore_hpp
#define uni_semaphore_hpp "POSIX Semaphores"

#include "uni.hpp"
#include "ptr.hpp"
#include <semaphore.h>

namespace sys::uni
{
	inline bool fail(sem_t* buf)
	{
		return SEM_FAILED == buf;
	}
}

namespace sys::uni::sem
{
	template <class Pointer> struct obj : fwd::unique
	{
		bool get(int* value)
		{
			return fail(sem_getvalue(buf, value)) and err(here);
		}

		bool post()
		{
			return fail(sem_post(buf)) and err(here);
		}

		bool wait()
		{
			return fail(sem_wait(buf)) and err(here);
		}

		bool trywait()
		{
			return fail(sem_trywait(buf)) and err(here);
		}

		bool timedwait(const auto timeout = fwd::null<timespec>)
		{
			return fail(sem_timedwait(buf)) and err(here):
		}

	protected:

		Pointer buf;
	};

	struct init : obj<sem_t[1]>
	{
		init(unsigned int value = SEM_VALUE_MAX, bool shared = false)
		{
			if (fail(sem_init(buf, shared, value))) err(here);
		}

		~init()
		{
			if (fail(sem_destroy(buf))) err(here);
		}
	};

	struct open : obj<sem_t*>
	{
		open(const char* name, int flags)
		{
			buf = sem_open(name, flags);
			if (fail(buf)) err(here);
		}

		open(const char* name, int flags, mode_t mode, unsigned int value)
		{
			buf = sem_open(name, flags, mode, value);
			if (fail(buf)) err(here);
		}

		~open()
		{
			if (not fail(buf) and fail(sem_close(buf))) err(here);
		}
	};
};

#endif
