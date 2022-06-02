#ifndef uni_signal_hpp
#define uni_signal_hpp "POSIX Signals"

#include "uni.hpp"
#include "err.hpp"
#include "ptr.hpp"
#include "doc.hpp"
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

		set(std::initializer_list<int> list)
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
			return fail(sigaltstack(this, prev)) and sys::err(here);
		}

		bool get(const stack_t* next = nullptr)
		{
			return fail(sigaltstack(next, this)) and sys::err(here);
		}

		auto alloc(size_t size)
		{
			constexpr size_t limit = SIGSTKSZ;
			ss_size = std::max(size, limit);
			ss_sp = std::realloc(ss_sp, ss_size);
			return ss_size;
		}
	};

	struct action : fwd::no_copy, sigaction
	{
		action(int flags, struct set mask)
		{
			sa_flags = flags;
			sa_mask = mask.buf[0];
			sa_sigaction = nullptr;
			sa_handler = nullptr;
		}

		bool set(int n, struct sigaction* prev = nullptr) const
		{
			return fail(::sigaction(n, this, prev)) and sys::err(here);
		}

		bool get(int n, const struct sigaction* next = nullptr)
		{
			return fail(::sigaction(n, next, this)) and sys::err(here);
		}
	};

	struct info : sig::action
	{
		info(int flags, struct set mask) : action(SA_SIGINFO | flags, mask)
		{
			sa_sigaction = queue;
		}

	private:

		static void queue(int signo, siginfo_t* info, void* context)
		{
			const int fd = info->si_value.sival_int;
			if (STDERR_FILENO == fd)
			{
				psiginfo(info, strsignal(signo));
			}
			else
			{
				const auto tmp = sys::dup(STDERR_FILENO);
				sys::dup2(fd, STDERR_FILENO);
				psiginfo(info, strsignal(signo));
				sys::dup2(tmp, STDERR_FILENO);
			}
			(void) context;
		}
	};

	inline bool queue(pid_t pid, int n, int fd = STDERR_FILENO)
	{
		sigval value;
		value.sival_int = fd;
		const auto error = fail(sigqueue(pid, n, value));
		if (error) sys::err(here);
		return error;
	}

	struct event : sigevent
	{
		event(fwd::event f, pthread_attr_t* attr = nullptr)
		{
			sigev_value.sival_int = doc::signal(f);
			sigev_notify = SIGEV_THREAD;
			sigev_notify_function = thread;
			sigev_notify_attributes = attr;
		}

	private:

		static void thread(sigval sigev_value)
		{
			doc::raise(sigev_value.sival_int);
		}
	};
}

namespace sys::uni::time
{
	struct event : sig::event
	{
		timer_t id;

		event(fwd::event f, pthread_attr_t* attr = nullptr, clockid_t clock = CLOCK_REALTIME)
		: sig::event(f, attr)
		{
			if (fail(timer_create(clock, this, &id)))
			{
				sys::err(here, "timer_create");
			}
		}

		~event()
		{
			if (fail(timer_delete(id)))
			{
				sys::err(here, "timer_delete");
			}
		}
	};
}

namespace sys::uni::msg
{
	struct event : sig::event
	{
		event(fwd::event f, mqd_t mqd, pthread_attr_t* attr = nullptr)
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
