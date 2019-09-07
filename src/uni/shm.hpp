#ifndef uni_shm_hpp
#define uni_shm_hpp

#include "uni.hpp"
#include "err.hpp"
#include "ptr.hpp"
#include <sys/types.h>
#include <sys/shm.h>

namespace sys::uni
{
	auto make_shm(int id, int flag = 0, void *ptr = nullptr)
	{
		ptr = shmat(id, ptr, flag);
		if (nullptr == ptr)
		{
			err(here, "shmat");
		}

		return make_ptr(ptr, [](auto ptr)
		{
			if (nullptr != ptr)
			{
				if (fail(shmdt(ptr)))
				{
					err(here, "shmdt");
				}
			}
		});
	}

	struct shmds : shmid_ds
	{
		bool get(int id)
		{
			if (fail(shmctl(id, IPC_STAT, this)))
			{
				err(here, "shmctl", id);
				return failure;
			}
			return success;
		}

		bool set(int id)
		{
			if (fail(shmctl(id, IPC_SET, this)))
			{
				err(here, "shmctl", id);
				return failure;
			}
			return success;
		}
	};

	struct shmid
	{
		operator int() const
		{
			return id;
		}

		shmid(size_t sz, int flags = 0, key_t key = IPC_PRIVATE)
		{
			id = shmget(key, sz, flags);
			if (fail(id))
			{
				err(here, "shmget", sz, flags);	
			}
		}

		~shmid()
		{
			if (not fail(id))
			{
				if (fail(shmctl(id, IPC_RMID, nullptr)))
				{
					err(here, "shmctl", id);
				}
			}
		}

	private:

		int id;
	};
}

#endif // file
