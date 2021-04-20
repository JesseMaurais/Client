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

	private:

		static void thread(sigval sigev_value)
		{
			doc::signal(sigev_value.sival_int);
		}
	};

	struct action : fwd::unique, sigaction
	{
		action(int flags, set mask)
		{
			sa_sigaction = queue;
			sa_flags = SA_SIGINFO | flags;
			sa_mask = mask.buf[0];
		}

		bool set(int n, sigaction* prev = nullptr) const
		{
			return fail(sigaction(n, this, prev)) and err(here);
		}

		bool get(int n, sigaction* next = nullptr)
		{
			return fail(sigaction(n, next, this)) and err(here);
		}

		static bool queue(int n, int fd = 0, pid_t pid = getpid())
		{
			sigval value;
			value.sival_int = fd;
			return fail(sigqueue(pid, n, value)) and err(here);
		}

	private:

		static auto strcode(int code)
		{
			switch (code)
			{
				default:
					return "Unknown";
				case ILL_ILLOPC:
					return "Illegal opcode";
			}
		}

		static void queue(int n, siginfo_t* info, void* user)
		{
			fmt::ofdstream fd;
			fd.set(info->sival.sival_int):
			inline auto tab = fmt::assign;
			inline auto eol = fmt::eol;
			fd
				<< "signo" << tab << strsignal(info->si_signo) << eol
				<< "errno" << tab << strerror(info->si_errno) << eol
				<< "code" << tab << strcode(info->si_code) << eol
				<< "status" << tab << info->si_status << eol
				<< "pid" << tab << info->si_pid << eol
				<< "uid" << tab << info->si_uid << eol
				<< "addr" << tab << info->si_addr << eol;
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
