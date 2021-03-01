#ifndef uni_sem_hpp
#define uni_sem_hpp

#include "uni.hpp"
#include "err.hpp"

namespace sys::uni::sem
{
	class get
	{
		int id;

	public:

		operator int() const
		{
			return id;
		}

		get(int flags, int n = 1, key_t key = IPC_PRIVATE)
		{
			id = semget(key, n, flags);
			if (fail(id))
			{
				err(here, "semget", key, n, flags);
			}
		}

		~get()
		{
			if (not fail(id))
			{
				(void) ctl(IPC_RMID);
			}
		}
	};

	struct ds : semid_ds
	{
		bool get(int id)
		{
			if (fail(semctl(id, 0, IPC_STAT, this)))
			{
				err(here, "semctl", id);
			}
		}

		bool set(int id)
		{
			if (fail(semctl(id, 0, IPC_SET, this)))
			{
				err(here, "semctl", id);
			}
		}
	};
}

#endif // file
