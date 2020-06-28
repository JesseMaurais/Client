#ifndef uni_signal_hpp
#define uni_signal_hpp

#include "uni.hpp"
#include "sys.hpp"
#include "ptr.hpp"
#include "err.hpp"
#include <signal.h>
#include <pthread.h>
#include <functional>

namespace sys::uni::sig
{
	struct set
	{
		sigset_t obj;
	};

	struct event : unique, sigevent
	{
		using signature = void();
		using observer = std::function<signature>;

		event(observer ob, int type, pthread_attr_t* attr = nullptr)
		: go(ob)
		{
			sigev_notify = type;
			sigev_notify_function = thunk;
			sigev_notify_attributes = attr;
			sigev_value.sival_ptr = this;
		}

	private:

		observer go;

		static void thunk(sigval sv)
		{
			auto that = reinterpret_cast<event*>(sv.sival_ptr);
			that->go();
		}
	};

	struct timer : event
	{
		timer_t id;

		timer(observer ob, int type = SIGEV_THREAD, clockid_t clock = CLOCK_REALTIME)
		: event(ob, type)
		{
			if (fail(timer_create(clock, this, &id)))
			{
				sys::err(here);
			}
		}

		~timer()
		{
			if (fail(timer_delete(id)))
			{
				sys::err(here);
			}
		}
	};
}

#endif // file
