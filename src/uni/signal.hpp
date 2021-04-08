#ifndef uni_signal_hpp
#define uni_signal_hpp "POSIX Signals"

#include "uni.hpp"
#include "err.hpp"
#include "ptr.hpp"
#include <time.h>
#include <mqueue.h>
#include <signal.h>
#include <pthread.h>
#include <functional>

namespace sys::uni::sig
{
	struct set
	{
		sigset_t buf[1];

		bool empty()
		{
			return fail(sigemptyset(buf)) and sys::err(here);
		}

		bool fill()
		{
			return fail(sigfillset(buf)) and sys::err(here);
		}

		bool add(int signo)
		{
			return fail(sigaddset(buf, signo)) and sys::err(here);
		}

		bool del(int signo)
		{
			return fail(sigdelset(buf, signo)) and sys::err(here);
		}

		bool member(int signo) const
		{
			const int no = sigismember(buf, signo);
			if (fail(no)) sys::err(here);
			return no;
		}
	};

	struct event : fwd::unique, sigevent
	{
		using function = std::function<void()>;

		event(function f, pthread_attr_t* attr = nullptr) : work(f)
		{
			sigev_value.sival_ptr = this;
			sigev_notify = SIGEV_THREAD;
			sigev_notify_function = thread;
			sigev_notify_attributes = attr;
		}

	private:

		function work;

		static void thread(sigval sigev_value)
		{
			auto that = fwd::cast_as<event>(sigev_value.sival_ptr);
			if (that) that->work();
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
	struct event : sig::event
	{
		event(function f, mqd_t mqd, pthread_attr_t* attr = nullptr) 
		: sig::event(f, attr)
		{
			if (fail(mq_notify(mqd, this)))
			{
				sys::err(here, "mq_notify");
			}
		}
	};
}

#endif // file
