#ifndef uni_msg_hpp
#define uni_msg_hpp

#include "uni.hpp"
#include "err.hpp"
#include "ptr.hpp"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

namespace sys::uni::msg
{
	class get
	{
		int id;

	public:

		operator int() const
		{
			return id;
		}

		get(int flags, key_t key = IPC_PRIVATE)
		{
			id = msgget(key, flags);
			if (fail(id))
			{
				err(here, "msgget", key, flags);
			}
		}

		~get()
		{
			if (not fail(id))
			{
				if (fail(msgctl(id, IPC_RMID, nullptr)))
				{
					err(here, "msgctl", id);
				}
			}
		}

		ssize_t rcv(void* buf, size_t sz, int type, int flags) const
		{
			auto const n = msgrcv(buf, sz, type, flags);
			if (fail(n))
			{
				err(here, "msgrcv", sz, type, flags);
			}
			return n;
		}

		ssize_t snd(void* buf, size_t sz, int flags) const
		{
			if (fail(msgsnd(id, buf, sz, flags)))
			{
				err(here, "msgsnd", sz, flags);
				return invalid;
			}
			return sz;
		}
	};

	struct ds : msqid_ds
	{
		bool get(int id)
		{
			if (fail(msgctl(id, IPC_STAT, this)))
			{
				err(here, "msgctl", id);
				return failure;
			}
			return success;
		}

		bool set(int id)
		{
			if (fail(msgctl(id, IPC_SET, this)))
			{
				err(here, "msgctl", id);
				return failure;
			}
			return success;
		}
	};
}

#endif // file
