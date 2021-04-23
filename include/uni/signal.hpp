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
			sigev_value.sival_int = doc::socket(f);
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

	struct action : fwd::unique, fwd::zero<sigaction>
	{
		using act = fwd::as_ptr<sigaction>;
		using set = fwd::as_ptr<sigset_t>;
		using info = fwd::as_ptr<siginfo_t>;
		using context = fwd::as_ptr<void>;
		using event::function;

		action(int flags, set mask)
		{
			sa_sigaction = queue;
			sa_flags = SA_SIGINFO | flags;
			sa_mask = *mask;
		}

		bool set(int no, act prev = nullptr) const
		{
			return fail(sigaction(no, this, prev)) and err(here);
		}

		bool get(int no, act next = nullptr)
		{
			return fail(sigaction(no, next, this)) and err(here);
		}

	private:

		static void queue(int no, info that, context user)
		{
			#ifdef assert
			assert(SI_QUEUE == that->si_code);
			#endif
			doc::signal(that->sival.sival_int);
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
