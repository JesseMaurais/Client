#ifndef uni_signal_hpp
#define uni_signal_hpp "POSIX Signals"

#include "uni.hpp"
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

		auto ismember(int n) const
		{
			auto err = sigismember(buf, n);
			if (fail(err))
			{
				perror("sigismember");
			}
			return err;
		}

		auto add(int n)
		{
			auto err = sigaddset(buf, n);
			if (fail(err))
			{
				perror("sigaddset");
			}
			return err;
		}

		auto del(int n)
		{
			auto err = sigdelset(buf, n);
			if (fail(err))
			{
				perror("sigdelset");
			}
			return err;
		}

		auto empty()
		{
			auto err = sigemptyset(buf);
			if (fail(err))
			{
				perror("sigemptyset");
			}
			return err;
		}

		auto fill()
		{
			auto err = sigfillset(buf);
			if (fail(err))
			{
				perror("sigfillset");
			}
			return err;
		}

		auto pending()
		{
			auto err = sigpending(buf);
			if (fail(err))
			{
				perror("sigpending");
			}
			return err;
		}

		auto suspend() const
		{
			auto err = sigsuspend(buf);
			if (fail(err))
			{
				perror("sigsuspend");
			}
			return err;
		}

		auto wait(int* n) const
		{
			auto err = sigwait(buf, n);
			if (fail(err))
			{
				perror("sigwait");
			}
			return err;
		}

		auto wait(siginfo_t* info = nullptr) const
		{
			auto err = sigwaitinfo(buf, info);
			if (fail(err))
			{
				perror("sigwaitinfo");
			}
			return err;
		}

		auto procmask(int how, sigset_t* old = nullptr) const
		{
			auto err = sigprocmask(how, buf, old);
			if (fail(err))
			{
				perror("sigprocmask");
			}
			return err;
		}

		auto threadmask(int how, sigset_t* old = nullptr) const
		{
			auto err = pthread_sigmask(how, buf, old);
			if (fail(err))
			{
				perror("pthread_sigmask");
			}
			return err;
		}

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
	};

	struct stack : fwd::zero<stack_t>
	{
		auto set(stack_t* prev = nullptr) const
		{
			auto err = sigaltstack(this, prev);
			if (fail(err))
			{ 
				perror("sigaltstack");
			}
			return err;
		}

		auto get(const stack_t* next = nullptr)
		{
			auto err = sigaltstack(next, this);
			if (fail(err))
			{
				perror("sigaltstack");
			}
			return err;
		}

		auto alloc(size_t size)
		{
			const size_t limit = SIGSTKSZ;
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

		auto set(int n, struct sigaction* prev = nullptr) const
		{
			auto err = ::sigaction(n, this, prev);
			if (fail(err))
			{
				perror("sigaction");
			}
			return err;
		}

		auto get(int n, const struct sigaction* next = nullptr)
		{
			auto err = ::sigaction(n, next, this);
			if (fail(err))
			{
				perror("sigaction");
			}
			return err;
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

	inline auto queue(pid_t pid, int n, int fd = STDERR_FILENO)
	{
		sigval value;
		value.sival_int = fd;
		auto err = sigqueue(pid, n, value);
		if (fail(err))
		{
			perror("sigqueue");
		}
		return err;
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
				perror("timer_create");
			}
		}

		~event()
		{
			if (fail(timer_delete(id)))
			{
				perror("timer_delete");
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
				perror("mq_notify");
			}
		}
	};
}

#endif // file
