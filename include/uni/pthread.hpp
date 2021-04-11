#ifndef uni_pthread_hpp
#define uni_pthread_hpp "POSIX Threads"

#include "uni.hpp"
#include "sys.hpp"
#include "ptr.hpp"
#include "err.hpp"
#include "tmp.hpp"
#include "signal.hpp"
#include "mqueue.hpp"

namespace sys::uni
{
	struct start : fwd::unique
	{
		pthread_t id;

		start(sig::event::function f, pthread_attr_t* attr = nullptr) : work(f)
		{
			const int no = pthread_create(&id, attr, thread, this);
			if (no) err(no, here);
		}

		~start()
		{
			auto that = fwd::void_ptr;
			const int no = pthread_join(id, &that);
			if (no) err(no, here);

			#ifdef assert
			assert(no or this == that);
			#endif
		}

	private:

		sig::event::function work;

		static void* thread(void* ptr)
		{
			auto that = fwd::cast_as<start>(ptr);
			if (that) that->work();
			return ptr;
		}
	};

	struct thread : fwd::unique
	{
		pthread_attr_t buf[1];

		auto start(sig::event::function f)
		{
			return sys::uni::start(f, buf);
		}

		auto event(sig::event::function f)
		{
			return sig::event(f, buf);
		}

		auto notify(sig::event::function f, mqd_t mqd)
		{
			return msg::event(f, mqd, buf);
		}

		auto timer(sig::event::function f, clockid_t clock = CLOCK_REALTIME)
		{
			return time::event(f, clock, buf);
		}

		thread()
		{
			if (const int no = pthread_attr_init(buf); no) err(no, here);
		}

		~thread()
		{
			if (const int no = pthread_attr_destroy(buf); no) err(no, here);
		}

		int getdetachstate(int* state) const
		{
			const int no = pthread_attr_getdetachstate(buf, state);
			if (no) err(no, here);
			return no;
		}

		int setdetachstate(int state)
		{
			const int no = pthread_attr_setdetachstate(buf, state);
			if (no) err(no, here);
			return no;
		}

		int getguardsize(size_t* size) const
		{
			const int no = pthread_attr_getguardsize(buf, size);
			if (no) err(no, here);
			return no;
		}

		int setguardsize(size_t size)
		{
			const int no = pthread_attr_setguardsize(buf, size);
			if (no) err(no, here);
			return no;
		}

		int getscope(int* scope) const
		{
			const int no = pthread_attr_getscope(buf, scope);
			if (no) err(no, here);
			return no;
		}

		int setscope(int scope)
		{
			const int no = pthread_attr_setscope(buf, scope);
			if (no) err(no, here);
			return no;
		}

		int getstacksize(size_t* size) const
		{
			const int no = pthread_attr_getstacksize(buf, size);
			if (no) err(no, here);
			return no;
		}

		int setstacksize(size_t size)
		{
			const int no = pthread_attr_setstacksize(buf, size);
			if (no) err(no, here);
			return no;
		}
	};

	struct cond : fwd::unique
	{
		pthread_cond_t buf[1];

		int wait(pthread_mutex_t* mutex)
		{
			const int no = pthread_cond_wait(buf, mutex);
			if (no) err(no, here);
			return no;
		}

		int signal()
		{
			const int no = pthread_cond_signal(buf);
			if (no) err(no, here);
			return no;
		}

		int broadcast()
		{
			const int no = pthread_cond_broadcast(buf);
			if (no) err(no, here);
			return no;
		}

		cond(pthread_condattr_t const* attr = nullptr)
		{
			const int no = pthread_cond_init(buf, attr);
			if (no) err(no, here);
		}

		~cond()
		{
			const int no = pthread_cond_destroy(buf);
			if (no) err(no, here);
		}

		struct attr
		{
			pthread_condattr_t buf[1];

			operator cond() const
			{
				return cond(buf);
			}

			attr()
			{
				const int no = pthread_condattr_init(buf);
				if (no) err(no, here);
			}

			~attr()
			{
				const int no = pthread_condattr_destroy(buf);
				if (no) err(no, here);
			}

			int getpshared(int* pshared) const
			{
				const int no = pthread_condattr_getpshared(buf, pshared);
				if (no) err(no, here);
				return no;
			}

			int setpshared(int pshared)
			{
				const int no = pthread_condattr_setpshared(buf, pshared);
				if (no) err(no, here);
				return no;
			}
		};
	};

	struct mutex : fwd::unique
	{
		pthread_mutex_t buf[1];

		int getprioceiling(int* prio) const
		{
			const int no = pthread_mutex_getprioceiling(buf, prio);
			if (no) err(no, here);
			return no;
		}

		int setprioceiling(int prio, int* old = nullptr)
		{
			if (nullptr == old) old = &prio;
			const int no = pthread_mutex_setprioceiling(buf, prio, old);
			if (no) err(no, here);
			return no;
		}

		int lock()
		{
			const int no = pthread_mutex_lock(buf);
			if (no) err(no, here);
			return no;
		}

		int unlock()
		{
			const int no = pthread_mutex_unlock(buf);
			if (no) err(no, here);
			return no;
		}

		int trylock()
		{
			const int no = pthread_mutex_trylock(buf);
			if (no) err(no, here);
			return no;
		}

		int wait(cond& var)
		{
			return var.wait(buf);
		}

		mutex(pthread_mutexattr_t const* attr = nullptr)
		{
			const int no = pthread_mutex_init(buf, attr);
			if (no) err(no, here);
		}

		~mutex()
		{
			const int no = pthread_mutex_destroy(buf);
			if (no) err(no, here);
		}

		struct attr
		{
			pthread_mutexattr_t buf[1];

			operator mutex() const
			{
				return mutex(buf);
			}

			attr()
			{
				const int no = pthread_mutexattr_init(buf);
				if (no) err(no, here);
			}

			~attr()
			{
				const int no = pthread_mutexattr_destroy(buf);
				if (no) err(no, here);
			}

			int getprioceiling(int* prio) const
			{
				const int no = pthread_mutexattr_getprioceiling(buf, prio);
				if (no) err(no, here);
				return no;
			}

			int setprioceiling(int prio)
			{
				const int no = pthread_mutexattr_setprioceiling(buf, prio);
				if (no) err(no, here);
				return no;
			}

			int getprotocol(int* proto) const
			{
				const int no = pthread_mutexattr_getprotocol(buf, proto);
				if (no) err(no, here);
				return no;
			}

			int setprotocol(int proto)
			{
				const int no = pthread_mutexattr_setprotocol(buf, proto);
				if (no) err(no, here);
				return no;
			}

			int getpshared(int* pshared) const
			{
				const int no = pthread_mutexattr_getpshared(buf, pshared);
				if (no) err(no, here);
				return no;
			}

			int setpshared(int pshared)
			{
				const int no = pthread_mutexattr_setpshared(buf, pshared);
				if (no) err(no, here);
				return no;
			}

			int gettype(int* type) const
			{
				const int no = pthread_mutexattr_gettype(buf, type);
				if (no) err(no, here);
				return no;
			}

			int settype(int type)
			{
				const int no = pthread_mutexattr_settype(buf, type);
				if (no) err(no, here);
				return no;
			}
		};
	};

	struct rwlock : fwd::unique
	{
		pthread_rwlock_t buf[1];

		int rdlock()
		{
			const int no = pthread_rwlock_rdlock(buf);
			if (no) err(no, here);
			return no;
		}

		int wrlock()
		{
			const int no = pthread_rwlock_wrlock(buf);
			if (no) err(no, here);
			return no;
		}

		int tryrdlock()
		{
			const int no = pthread_rwlock_tryrdlock(buf);
			if (no) err(no, here);
			return no;
		}

		int trywrlock()
		{
			const int no = pthread_rwlock_trywrlock(buf);
			if (no) err(no, here);
			return no;
		}

		int unlock()
		{
			const int no = pthread_rwlock_unlock(buf);
			if (no) err(no, here);
			return no;
		}

		rwlock(pthread_rwlockattr_t const* attr = nullptr)
		{
			const int no = pthread_rwlock_init(buf, attr);
			if (no) err(no, here);
		}

		~rwlock()
		{
			const int no = pthread_rwlock_destroy(buf);
			if (no) err(no, here);
		}

		struct attr
		{
			pthread_rwlockattr_t buf[1];

			operator rwlock() const
			{
				return rwlock(buf);
			}

			attr()
			{
				const int no = pthread_rwlockattr_init(buf);
				if (no) err(no, here);
			}

			~attr()
			{
				const int no = pthread_rwlockattr_destroy(buf);
				if (no) err(no, here);
			}

			int getpshared(int* pshared) const
			{
				const int no = pthread_rwlockattr_getpshared(buf, pshared);
				if (no) err(no, here);
				return no;
			}

			int setpshared(int pshared)
			{
				const int no = pthread_rwlockattr_setpshared(buf, pshared);
				if (no) err(no, here);
				return no;
			}
		};
	};
}

namespace sys
{
	using thread = uni::start;

	struct mutex : uni::mutex
	{
		auto lock()
		{
			class unlock : fwd::unique
			{
				uni::mutex* that;

			public:

				unlock(mutex* ptr) : that(ptr)
				{
					that->lock();
				}

				~unlock()
				{
					that->unlock();
				}

			};
			return unlock(this);
		}
	};

	struct rwlock : uni::rwlock
	{
		auto read()
		{
			class unlock : fwd::unique
			{
				uni::rwlock* that;

			public:

				unlock(uni::rwlock* ptr) : that(ptr)
				{
					that->rdlock();
				}

				~unlock()
				{
					that->unlock();
				}
			};
			return unlock(this);
		}

		auto write()
		{
			class unlock : fwd::unique
			{
				uni::rwlock* that;

			public:
			
				unlock(uni::rwlock* ptr) : that(ptr)
				{
					that->wrlock();
				}

				~unlock()
				{
					that->unlock();
				}
			};
			return unlock(this);
		}
	};
}

#endif // file
