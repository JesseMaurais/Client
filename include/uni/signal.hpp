#ifndef uni_signal_hpp
#define uni_signal_hpp "POSIX Signals"

#include "uni.hpp"
#include "err.hpp"
#include "ptr.hpp"
#include "msg.hpp"
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

		bool member(int signo) const
		{
			const int no = sigismember(buf, signo);
			if (fail(no)) sys::err(here);
			return no;
		}

		bool add(int signo)
		{
			return fail(sigaddset(buf, signo)) and sys::err(here);
		}

		bool del(int signo)
		{
			return fail(sigdelset(buf, signo)) and sys::err(here);
		}

		bool empty()
		{
			return fail(sigemptyset(buf)) and sys::err(here);
		}

		bool fill()
		{
			return fail(sigfillset(buf)) and sys::err(here);
		}

		bool pending()
		{
			return fail(sigpending(buf)) and sys::err(here);
		}

		bool suspend() const
		{
			return fail(sigsuspend(buf)) and sys::err(here);
		}

		bool wait(int* signo) const
		{
			return fail(sigwait(buf, signo)) and sys::err(here);
		}

		bool wait(siginfo_t* info = nullptr) const
		{
			return fail(sigwaitinfo(buf, info)) and sys::err(here);
		}

		bool procmask(int how, sigset_t* old = nullptr) const
		{
			return fail(sigprocmask(how, buf, old)) and sys::err(here);
		}

		bool threadmask(int how, sigset_t* old = nullptr) const
		{
			return fail(pthread_sigmask(how, buf, old)) and sys::err(here);
		}
	};

	struct event : fwd::unique, sigevent
	{
		using doc::function;

		event(function f, pthread_attr_t* attr = nullptr) : work(f)
		{
			sigev_value.sival_int = doc::socket().emplace(f);
			sigev_notify = SIGEV_THREAD;
			sigev_notify_function = thread;
			sigev_notify_attributes = attr;
		}

	private:

		static void thread(sigval sigev_value)
		{
			doc::signal(sigev_value.sival_int);
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
