#ifndef uni_shm_hpp
#define uni_shm_hpp

#include "uni.hpp"
#include "err.hpp"
#include "ptr.hpp"
#include <sys/types.h>
#include <sys/shm.h>

namespace sys::uni::shm
{
	auto at(int id, int flag = 0, void *ptr = nullptr)
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

	class get
	{
		int id;

	public:

		operator int() const
		{
			return id;
		}

		get(size_t sz, int flags, key_t key = IPC_PRIVATE)
		{
			id = shmget(key, sz, flags);
			if (fail(id))
			{
				err(here, "shmget", key, sz, flags);	
			}
		}

		~get()
		{
			if (not fail(id))
			{
				if (fail(shmctl(id, IPC_RMID, nullptr)))
				{
					err(here, "shmctl", id);
				}
			}
		}
	};

	struct ctl : shmid_ds
	{
		bool get(int id)
		{
			if (sys::fail(shmctl(id, IPC_STAT, this)))
			{
				err(here, "shmctl", id);
				return failure;
			}
			return success;
		}

		bool set(int id)
		{
			if (sys::fail(shmctl(id, IPC_SET, this)))
			{
				err(here, "shmctl", id);
				return failure;
			}
			return success;
		}
	};
}

#endif // file
