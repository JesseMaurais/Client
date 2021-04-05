#ifndef uni_signal_hpp
#define uni_signal_hpp "POSIX Signals"

#include "uni.hpp"
#include "err.hpp"
#include "ptr.hpp"
#include "msg.hpp"
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <functional>

namespace sys::uni::sig
{
	struct event : fwd::unique, sigevent
	{
		using function = std::function<void()>;

		event(function f, pthread_attr_t* attr = nullptr) : work(f)
		{
			sigev_value.sival_int = doc::msg().make(f);
			sigev_notify = SIGEV_THREAD;
			sigev_notify_function = thunk;
			sigev_notify_attributes = attr;
		}

		~event()
		{
			doc::msg().free(sigev_value.sival_int);
		}

	private:

		static void thunk(sigval sigev_value)
		{
			auto that = doc::msg().find(sigev_value.sival_int);
			if (that) doc::msg().at(sigev_value.sival_int)():
		}
	};
}

namespace sys::uni::time
{
	struct event : sig::event
	{
		timer_t id;

		event(function f, clockid_t clock = CLOCK_REALTIME, pthread_attr_t* attr = nullptr) 
		: sig::event(f, attr)
		{
			if (fail(timer_create(clock, this, &id)))
			{
				err(here, "timer_create");
			}
		}

		~event()
		{
			if (fail(timer_delete(id)))
			{
				err(here, "timer_delete");
			}
		}
	};
}

namespace sys::uni::msg
{
	struct event : sys::uni::sig::event
	{
		event(function f, mqd_t mqd, pthread_attr_t* attr = nullptr) 
		: sig::event(f, attr)
		{
			const int err = mq_notify(mqd, this);
			if (fail(err))
			{
				err(here, "mq_notify");
			}
		}
	};
}

#endif // file
