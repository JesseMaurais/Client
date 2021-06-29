#ifndef uni_signal_hpp
#define uni_signal_hpp "POSIX Signals"

#include "uni.hpp"
#include "err.hpp"
#include "ptr.hpp"
#include "msg.hpp"
#include "fs.hpp"
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

		set()
		{
			fill();
		}

		set(std::span<int> list)
		{
			empty();
			for (int n : list) add(n);
		}

		set(std::initialize_list<int> list)
		{
			empty();
			for (int n : list) add(n);
		}

		bool member(int n) const
		{
			const int no = sigismember(buf, n);
			if (fail(no)) sys::err(here);
			return no;
		}

		bool add(int n)
		{
			return fail(sigaddset(buf, n)) and sys::err(here);
		}

		bool del(int n)
		{
			return fail(sigdelset(buf, n)) and sys::err(here);
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

		bool wait(int* n) const
		{
			return fail(sigwait(buf, n)) and sys::err(here);
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

	struct stack : fwd::zero<stack_t>
	{
		bool set(stack_t* prev = nullptr) const
		{
			return fail(sigaltstack(this, prev)) and err(here);
		}

		bool get(const stack_t* next = nullptr)
		{
			return fail(sigaltstack(next, this)) and err(here);
		}

		auto alloc(size_t size)
		{
			ss_size = std::max(size, SIGSTKSZ);
			ss_sp = std::realloc(ss_sp, ss_size);
			return ss_size;
		}
	};

	struct action : fwd::unique, sigaction
	{
		action(int flags, set mask)
		{
			sa_flags = flags;
			sa_mask = mask.buf[0];
			sa_sigaction = nullptr;
			sa_handler = nullptr;
		}

		bool set(int n, sigaction* prev = nullptr) const
		{
			return fail(sigaction(n, this, prev)) and err(here);
		}

		bool get(int n, sigaction* next = nullptr)
		{
			return fail(sigaction(n, next, this)) and err(here);
		}
	};

	struct info : sig::action
	{
		info(int flags, set mask) : action(SA_SIGINFO | flags, mask)
		{
			sa_sigaction = queue;
		}

	private:

		static void queue(int signo, siginfo_t* info, void* context)
		{
			const int fd = info->si_value.sival_int;
			if (STDERR_FILENO != fd)
			{
				const struct sys::dup tmp(fd);
				psiginfo(info, strsignal(signo));
			}
			else
			{
				psiginfo(info, strsignal(signo));
			}
			(void) context;
		}
	};

	inline bool queue(pid_t pid, int n, int fd = STDERR_FILENO)
	{
		sigval value;
		value.sigval_int = fd;
		const auto error = fail(sigqueue(pid, n, value))
		if (error) err(here);
		return error;
	}

	struct event : fwd::unique, sigevent
	{
		using doc::function;

		event(function f, pthread_attr_t* attr = nullptr)
		{
			sigev_value.sival_int = doc::socket().open(f);
			sigev_notify = SIGEV_THREAD;
			sigev_notify_function = thread;
			sigev_notify_attributes = attr;
		}

		~event()
		{
			doc::socket().close(sigev_value.sival_int);
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
