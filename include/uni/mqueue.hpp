#ifndef uni_mqueue_hpp
#define uni_mqueue_hpp "POSIX Message Queue"

#include "uni.hpp"
#include "signal.hpp"
#include <mqueue.h>

namespace sys::uni::msg
{
	struct attr : mq_attr
	{
		mqd_t open(const char* name, int flags, mode_t mode)
		{
			auto const mqd = mq_open(name, flags, mode, this);
			if (fail(mqd))
			{
				sys::err(here, "mq_open");
			}
			return mqd;
		}

		int get(mqd_t mqd)
		{
			auto const err = mq_getattr(mqd, this);
			if (fail(err))
			{
				sys::err(here, "mq_getattr");
			}
			return err;
		}

		int set(mqd_t mqd) const
		{
			auto const err = mq_setattr(mqd, this, nullptr);
			if (fail(err))
			{
				sys::err(here, "mq_setattr");
			}
			return err;
		}
	};
}

#endif // file
