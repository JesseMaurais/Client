#ifndef uni_semaphore_hpp
#define uni_semaphore_hpp "POSIX Semaphores"

#include "uin.hpp"
#include "ptr.hpp"
#include <semaphore.h>

namespace sys::uni::sem
{
	static auto make_ptr(sem_t* sem)
	{
		return fwd::make_ptr(sem, [](auto sem)
		{
			if (SEM_FAILED != sem)
			{
				if (sys::fail(sem_destroy(sem)))
				{
					sys::uni::err(here, "sem_destroy");
				}
			}
		});
	}

	static auto open(const char* name, int flag)
	{
		const auto sem = sem_open(name, flag, mode);
		if (SEM_FAILED == sem)
		{
			sys::uni::err(here, "sem_open");
		}
		return make_ptr(sem);
	}

	static auto open(const char* name, int flag, mode_t mode, unsigned int value)
	{
		const auto sem = sem_open(name, flag, mode, value);
		if (SEM_FAILED == sem)
		{
			sys::uni::err(here, "sem_open");
		}
		return make_ptr(sem);
	}
};

#endif
