#ifndef uni_shm_hpp
#define uni_shm_hpp

#include "uni.hpp"
#include "err.hpp"
#include "ptr.hpp"

namespace sys::uni
{
	extern "C"
	{
		#include <sys/types.h>
		#include <sys/shm.h>
	}

	template <typename C>
	auto make_shm(int id, int flag = 0, C const *ptr = nullptr)
	{
		ptr = shmat(id, ptr, flag);
		if (sys::fail(ptr))
		{
			sys::err(here, "shmat");
		}

		return std::shared_ptr(ptr, [](auto ptr)
		{
			if (nullptr != ptr)
			{
				if (sys::fail(shmdt(ptr)))
				{
					sys::err(here, "shmdt");
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
				sys::err(here, "shmctl", id);
				return failure;
			}
			return success;
		}

		bool set(int id)
		{
			if (fail(shmctl(id, IPC_SET, this)))
			{
				sys::err(here, "shmctl", id);
				return failure;
			}
			return success;
		}
	};

	struct shmid
	{
		int id;

		shmid(key_t key, size_t sz, int flag)
		{
			id = shmget(key, sz, flag);
			if (fail(id))
			{
				sys::err(here, "shmget", sz, flags);	
			}
		}

		~shmid()
		{
			if (not fail(id))
			{
				if (fail(shmctl(id, IPC_RMID, nullptr)))
				{
					sys::err(here, "shmctl", id);
				}
			}
		}
	};
}

#endif // file
