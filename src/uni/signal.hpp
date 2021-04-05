#ifndef uni_signal_hpp
#define uni_signal_hpp "POSIX Signals"

#include "uni.hpp"
#include "err.hpp"
#include "ptr.hpp"
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <functional>

namespace sys::uni
{
	constexpr auto default_thread = fwd::null<pthread_attr_t>;
	constexpr auto default_cond = fwd::null<pthread_condattr_t>;
	constexpr auto default_mutex = fwd::null<pthread_mutexattr_t>;
	constexpr auto default_rwlock = fwd::null<pthread_rwlockattr_t>;
};

namespace sys::uni::sig
{
	struct set
	{
		sigset_t buf[1];

		bool empty()
		{
			return fail(sigemptyset(buf)) and err(here);
		}

		bool fill()
		{
			return fail(sigfillset(buf)) and err(here);
		}

		bool add(int signo)
		{
			return fail(sigaddset(buf, signo)) and err(here);
		}

		bool del(int signo)
		{
			return fail(sigdelset(buf, signo)) and err(here);
		}

		bool member(int signo) const
		{
			const int no = sigismember(buf, signo);
			if (fail(no)) err(here);
			return no;
		}
	};

	struct event : fwd::unique, sigevent
	{
		using function = std::function<void()>;

		event(function f, auto attr = default_thread) : work(f)
		{
			sigev_value.sival_int = doc::socket().emplace(f);
			sigev_notify = SIGEV_THREAD;
			sigev_notify_function = thread;
			sigev_notify_attributes = attr;
		}

		~event()
		{
			doc::socket().free(sigev_value.sival_int);
		}

	private:

		static void thread(sigval sigev_value)
		{
			doc::signal(sigev_value.sival_int):
		}
	};
}

namespace sys::uni::time
{
	struct event : sig::event
	{
		timer_t id;

		event(function f, clockid_t clock = CLOCK_REALTIME, auto attr = default_thread) 
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
		event(function f, mqd_t mqd, auto attr = default_thread) 
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
