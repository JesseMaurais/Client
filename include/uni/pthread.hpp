#ifndef pthread_hpp
#define pthread_hpp "POSIX Threads"

#include "uni.hpp"
#include "sys.hpp"
#include "ptr.hpp"
#include "tmp.hpp"
#include "signal.hpp"

namespace sys::uni
{
	struct start : fwd::no_copy
	{
		pthread_t id;

		start(fwd::event f, pthread_attr_t* attr = nullptr) : work(f)
		{
			if ((errno = pthread_create(&id, attr, thread, this)))
			{
				perror("pthread_create");
			}
		}

	private:

		fwd::event work;

		static void* thread(void* ptr)
		{
			auto that = fwd::cast_as<start>(ptr);
			if (that) that->work();
			return ptr;
		}
	};

	struct join : start
	{
		~join()
		{
			auto that = fwd::null<void>;
			if ((errno = pthread_join(id, &that)))
			{
				perror("pthread_join");
			}
			#ifdef assert
			assert(this == that);
			#endif
		}
	};

	struct thread : fwd::no_copy
	{
		pthread_attr_t buf[1];

		operator pthread_attr_t*()
		{
			return buf;
		}

		auto start(fwd::event f)
		{
			return sys::uni::start(f, buf);
		}

		auto event(fwd::event f)
		{
			return sig::event(f, buf);
		}

		auto notify(fwd::event f, mqd_t mqd)
		{
			return msg::event(f, mqd, buf);
		}

		auto timer(fwd::event f, clockid_t clock = CLOCK_REALTIME)
		{
			return time::event(f, buf, clock);
		}

		thread()
		{
			if ((errno = pthread_attr_init(buf)))
			{
				perror("pthread_attr_init");
			}
		}

		~thread()
		{
			if ((errno = pthread_attr_destroy(buf)))
			{
				perror("pthread_attr_destroy");
			}
		}

		auto getdetachstate(int* state) const
		{
			if ((errno = pthread_attr_getdetachstate(buf, state)))
			{
				perror("pthread_attr_getdetachstate");
			}
			return errno;
		}

		auto setdetachstate(int state)
		{
			if ((errno = pthread_attr_setdetachstate(buf, state)))
			{
				perror("pthread_attr_setdetachstate");
			}
			return errno;
		}

		auto getguardsize(size_t* size) const
		{
			if ((errno = pthread_attr_getguardsize(buf, size)))
			{
				perror("pthread_attr_getguardsize");
			}
			return errno;
		}

		auto setguardsize(size_t size)
		{
			if ((errno = pthread_attr_setguardsize(buf, size)))
			{
				perror("pthread_attr_setguardsize");
			}
			return errno;
		}

		auto getscope(int* scope) const
		{
			if ((errno = pthread_attr_getscope(buf, scope)))
			{
				perror("pthread_attr_getscope");
			}
			return errno;
		}

		auto setscope(int scope)
		{
			if ((errno = pthread_attr_setscope(buf, scope)))
			{
				perror("pthread_attr_setscope");
			}
			return errno;
		}

		auto getstacksize(size_t* size) const
		{
			if ((errno = pthread_attr_getstacksize(buf, size)))
			{
				perror("pthread_attr_getstacksize");
			}
			return errno;
		}

		int setstacksize(size_t size)
		{
			if ((errno = pthread_attr_setstacksize(buf, size)))
			{
				perror("pthread_attr_setstacksize");
			}
			return errno;
		}
	};

	struct cond : fwd::no_copy
	{
		pthread_cond_t buf[1];

		operator pthread_cond_t*()
		{
			return buf;
		}

		auto wait(pthread_mutex_t* mutex)
		{
			if ((errno = pthread_cond_wait(buf, mutex)))
			{
				perror("pthread_cond_wait");
			}
			return errno;
		}

		auto signal()
		{
			if ((errno = pthread_cond_signal(buf)))
			{
				perror("pthread_cond_signal");
			}
			return errno;
		}

		auto broadcast()
		{
			if ((errno = pthread_cond_broadcast(buf)))
			{
				perror("pthread_cond_broadcast");
			}
			return errno;
		}

		cond(pthread_condattr_t const* attr = nullptr)
		{
			if ((errno = pthread_cond_init(buf, attr)))
			{
				perror("pthread_cond_init");
			}
		}

		~cond()
		{
			if ((errno = pthread_cond_destroy(buf)))
			{
				perror("pthread_cond_destroy");
			}
		}

		struct attr
		{
			pthread_condattr_t buf[1];

			operator pthread_condattr_t*()
			{
				return buf;
			}

			operator cond() const
			{
				return cond(buf);
			}

			attr()
			{
				if ((errno = pthread_condattr_init(buf)))
				{
					perror("pthread_condattr_init");
				}
			}

			~attr()
			{
				if ((errno = pthread_condattr_destroy(buf)))
				{
					perror("pthread_condattr_destroy");
				}
			}

			auto getpshared(int* pshared) const
			{
				if ((errno = pthread_condattr_getpshared(buf, pshared)))
				{
					perror("pthread_condattr_getpshared");
				}
				return errno; 
			}

			auto setpshared(int pshared)
			{
				if ((errno = pthread_condattr_setpshared(buf, pshared)))
				{
					perror("pthread_condattr_setpshared");
				}
				return errno;
			}
		};
	};

	struct mutex : fwd::no_copy
	{
		pthread_mutex_t buf[1];

		operator pthread_mutex_t*()
		{
			return buf;
		}

		auto getprioceiling(int* prio) const
		{
			if ((errno = pthread_mutex_getprioceiling(buf, prio)))
			{
				perror("pthread_mutex_getprioceiling");
			}
			return errno;
		}

		auto setprioceiling(int prio, int* old = nullptr)
		{
			if (nullptr == old)
			{
				old = &prio;
			}
			if ((errno = pthread_mutex_setprioceiling(buf, prio, old)))
			{
				perror("pthread_mutex_setprioceiling");
			}
			return errno;
		}

		auto lock()
		{
			if ((errno = pthread_mutex_lock(buf)))
			{
				perror("pthread_mutex_lock");
			}
			return errno;
		}

		auto unlock()
		{
			if ((errno = pthread_mutex_unlock(buf)))
			{
				perror("pthread_mutex_unlock");
			}
			return errno;
		}

		auto trylock()
		{
			if ((errno = pthread_mutex_trylock(buf)))
			{
				perror("pthread_mutex_trylock");
			}
			return errno;
		}

		int wait(cond& var)
		{
			return var.wait(buf);
		}

		mutex(pthread_mutexattr_t const* attr = nullptr)
		{
			if ((errno = pthread_mutex_init(buf, attr)))
			{
				perror("pthread_mutex_init");
			}
		}

		~mutex()
		{
			if ((errno = pthread_mutex_destroy(buf)))
			{
				perror("pthread_mutex_destroy");
			}
		}

		struct attr
		{
			pthread_mutexattr_t buf[1];

			operator pthread_mutexattr_t*()
			{
				return buf;
			}

			operator mutex() const
			{
				return mutex(buf);
			}

			attr()
			{
				if ((errno = pthread_mutexattr_init(buf)))
				{
					perror("pthread_mutexattr_init");
				}
			}

			~attr()
			{
				if ((errno = pthread_mutexattr_destroy(buf)))
				{
					perror("pthread_mutexattr_destroy");
				}
			}

			auto getprioceiling(int* prio) const
			{
				if ((errno = pthread_mutexattr_getprioceiling(buf, prio)))
				{
					perror("pthread_mutexattr_getprioceiling");
				}
				return errno;
			}

			auto setprioceiling(int prio)
			{
				if ((errno = pthread_mutexattr_setprioceiling(buf, prio)))
				{
					perror("pthread_mutexattr_setprioceiling");
				}
				return errno;
			}

			auto getprotocol(int* proto) const
			{
				if ((errno = pthread_mutexattr_getprotocol(buf, proto)))
				{
					perror("pthread_mutexattr_getprotocol");
				}
				return errno;
			}

			auto setprotocol(int proto)
			{
				if ((errno = pthread_mutexattr_setprotocol(buf, proto)))
				{
					perror("pthread_mutexattr_setprotocol");
				}
				return errno;
			}

			auto getpshared(int* pshared) const
			{
				if ((errno = pthread_mutexattr_getpshared(buf, pshared)))
				{
					perror("pthread_mutexattr_getpshared");
				}
				return errno;
			}

			auto setpshared(int pshared)
			{
				if ((errno = pthread_mutexattr_setpshared(buf, pshared)))
				{
					perror("pthread_mutexattr_setpshared");
				}
				return errno;
			}

			auto gettype(int* type) const
			{
				if ((errno = pthread_mutexattr_gettype(buf, type)))
				{
					perror("pthread_mutexattr_gettype");
				}
				return errno;
			}

			auto settype(int type)
			{
				if ((errno = pthread_mutexattr_settype(buf, type)))
				{
					perror("pthread_mutexattr_settype");
				}
				return errno;
			}
		};
	};

	struct rwlock : fwd::no_copy
	{
		pthread_rwlock_t buf[1];

		operator pthread_rwlock_t*()
		{
			return buf;
		}

		auto rdlock()
		{
			if ((errno = pthread_rwlock_rdlock(buf)))
			{
				perror("pthread_rwlock_rdlock");
			}
			return errno;
		}

		auto wrlock()
		{
			if ((errno = pthread_rwlock_wrlock(buf)))
			{
				perror("pthread_rwlock_wrlock");
			}
			return errno;
		}

		auto tryrdlock()
		{
			if ((errno = pthread_rwlock_tryrdlock(buf)))
			{
				perror("pthread_rwlock_tryrdlock");
			}
			return errno;
		}

		auto trywrlock()
		{
			if ((errno = pthread_rwlock_trywrlock(buf)))
			{
				perror("pthread_rwlock_trywrlock");
			}
			return errno;
		}

		auto unlock()
		{
			if ((errno = pthread_rwlock_unlock(buf)))
			{
				perror("pthread_rwlock_unlock");
			}
			return errno;
		}

		rwlock(pthread_rwlockattr_t const* attr = nullptr)
		{
			if ((errno = pthread_rwlock_init(buf, attr)))
			{
				perror("pthread_rwlock_init");
			}
		}

		~rwlock()
		{
			if ((errno = pthread_rwlock_destroy(buf)))
			{
				perror("pthread_rwlock_destroy");
			}
		}

		struct attr
		{
			pthread_rwlockattr_t buf[1];

			operator pthread_rwlockattr_t*()
			{
				return buf;
			}

			operator rwlock() const
			{
				return rwlock(buf);
			}

			attr()
			{
				if ((errno = pthread_rwlockattr_init(buf)))
				{
					perror("pthread_rwlockattr_init");
				}
			}

			~attr()
			{
				if ((errno = pthread_rwlockattr_destroy(buf)))
				{
					perror("pthread_rwlockattr_destroy");
				}
			}

			auto getpshared(int* pshared) const
			{
				if ((errno = pthread_rwlockattr_getpshared(buf, pshared)))
				{
					perror("pthread_rwlockattr_getpshared");
				}
				return errno;
			}

			auto setpshared(int pshared)
			{
				if ((errno = pthread_rwlockattr_setpshared(buf, pshared)))
				{
					perror("pthread_rwlockattr_setpshared");
				}
				return errno;
			}
		};
	};
}

#endif // file
