#ifndef uni_semaphore_hpp
#define uni_semaphore_hpp

#include "uni.hpp"
#include <semaphore.h>

namespace sys::uni
{
	class semaphore
	{
		sem_t* ptr;

	public:

		operator sem_t*() const
		{
			return ptr;
		}

		semaphore(char const* name, int flag)
		{
			ptr = sem_open(name, flag);
			if (SEM_FAILED == ptr)
			{
				err(here, name, flag);
			}
		}

		semaphore(char const* name, int flag, mode_t mode, unsigned int value)
		{
			ptr = sem_open(name, flag, mode, value);
			if (SEM_FAILED == ptr)
			{
				err(here, name, flag, mode, value);
			}
		}

		~semaphore()
		{
			if (SEM_FAILED != ptr)
			{
				if (fail(sem_close(ptr)))
				{
					err(here);
				}
			}
		}
	};
}

namespace sys

#endif // file
