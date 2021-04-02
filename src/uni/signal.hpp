#ifndef uni_signal_hpp
#define uni_signal_hpp

#include "uni.hpp"
#include "err.hpp"
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <functional>

namespace sys::uni::sig
{
	struct event : fwd::unique, sigevent
	{
		using function = std::function<void()>;

		event(function f, pthread_attr_t* attr = nullptr)
		: work(f)
		{
			sigev_value.sival_ptr = this;
			sigev_notify = SEGEV_THREAD;
			sigev_notify_function = thunk;
			sigev_notify_attributes = attr;
		}

	private:

		function work;

		static void thunk(sigval sv)
		{
			auto that = static_cast<event*>(sv.sival_ptr);
			that->work();
		}
	};
}

namespace sys::uni::time
{
	struct event : sys::uni::sig::event
	{
		timer_t id;

		event(function f, clockid_t clock = CLOCK_REALTIME) : event(f)
		{
			if (fail(timer_create(clock, this, &id)))
			{
				sys::uni::err(here, "timer_create");
			}
		}

		~event()
		{
			if (fail(timer_delete(id)))
			{
				sys::uni::err(here, "timer_delete");
			}
		}
	};
}

#endif // file
