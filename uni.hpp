#ifndef uni_hpp
#define uni_hpp

#ifdef _WIN32
# error POSIX utility header included with wrong macro.
#endif

#include <pthread.h>
#include <dirent.h>
#include <cstring>
#include "sys.hpp"
#include "err.hpp"

namespace sys::uni
{
	struct files
	{
		DIR* ptr;

		files(char const *s)
		{
			ptr = opendir(s);
			if (ptr)
			{
				err(here, "opendir", s);
			}
		}

		~files()
		{
			if (ptr and fail(closedir(ptr)))
			{
				err(here, "closedir");
			}
		}

		auto operator++()
		{
			return readdir(ptr);
		}
	};

	template <typename... Args>
	inline void err(Args... args, int no)
	{
		sys::warn(args..., std::strerror(no);
	}

	using routine = void*(void*);

	struct thread : pthread_attr_t
	{
		mutable int no = 0;
		
		pthread_t init(routine* start, void* arg = nullptr) const
		{
			pthread_t id = sys::invalid;
			no = pthread_create(&id, this, start, arg);
			if (no) sys::uni::err(here, no);
			return id;
		}

		thread()
		{
			no = pthread_attr_init(this);
			if (no) sys::uni::err(here, no);
		}

		~thread()
		{
			if (not no)
			{
				no = pthread_attr_destroy(this);

			}
		}

		auto getdetachstate() const
		{
			int state;
			no = pthread_attr_getdetachstate(this, &state);
			return state;
		}

		void setdetachstate(int state)
		{
			no = pthread_attr_setdetachstate(this, state);
		}

		auto getguardsize() const
		{
			size_t size;
			no = pthread_attr_getguardsize(this, &size);
			return size;
		}

		void setguardsize(size_t size)
		{
			no = pthread_attr_setguardsize(this, size);
		}

		auto getscope() const
		{
			int scope;
			no = pthread_attr_getscope(this, &scope);
			return scope;
		}

		void setscope(int scope)
		{
			no = pthread_attr_setscope(this, scope);
		}

		auto getstackaddr() const
		{
			void* addr;
			no = pthread_attr_setstackaddr(this, &addr);
			return addr;
		}

		void setstackaddr(void* addr)
		{
			no = pthread_attr_setstackaddr(this, addr);
		}

		auto getstacksize() const
		{
			size_t size;
			no = pthread_attr_getstacksize(this, &size);
			return size;
		}

		void setstacksize(size_t size)
		{
			no = pthread_attr_setstacksize(this, size);
		}
	};

	struct cond : pthread_cond_t
	{
		mutable int no = 0;

		cond(pthread_condattr_t const* attr = nullptr)
		{
			no = pthread_cond_init(this, attr);
		}

		~cond()
		{
			if (not error)
			{
				no = pthread_cond_destroy(this);
			}
		}

		void wait(pthread_mutex_t* mutex = nullptr) const
		{
			no = pthread_cond_wait(this, mutex);
		}

		void signal() const
		{
			no = pthread_cond_signal(this);
		}

		void broadcast() const
		{
			no = pthread_cond_broadcast(this);
		}

		struct attr : pthread_condattr_t
		{
			cond init() const
			{
				return cond(this);
			}

			attr()
			{
				no = pthread_condattr_init(this);
			}

			~attr()
			{
				if (not no)
				{
					no = pthread_condattr_destroy(this);
				}
			}

			auto getpshared() const
			{
				int pshared;
				no = pthread_condattr_getpshared(this, &pshared);
				return pshared;
			}

			void setpshared(int pshared)
			{
				no = pthread_condattr_setpshared(this, pshared);
			}
		};
	};

	struct mutex : pthread_mutex_t
	{
		mutable int no = 0;

		mutex(pthread_mutexattr_t const* attr = nullptr)
		{
			no = pthread_mutex_init(this, attr);
		}

		~mutex()
		{
			if (not error)
			{
				no = pthread_mutex_destroy(this);
			}
		}

		auto getprioceiling() const
		{
			int prioceiling;
			no = pthread_mutex_getprioceiling(this, &prioceiling);
			return prioceiling;
		}

		void setprioceiling(int prioceiling)
		{
			no = pthread_mutex_setprioceiling(this, prioceiling);
		}

		void lock()
		{
			no = pthread_mutex_lock(this);
		}

		void unlock()
		{
			no = pthread_mutex_lock(this);
		}

		void trylock()
		{
			no = pthread_mutex_trylock(this);
		}

		struct attr : pthread_mutexattr_t
		{
			mutable int no = 0;

			mutex init() const
			{
				return mutex(this);
			}

			attr()
			{
				no = pthread_mutexattr_init(this);
			}

			~attr()
			{
				if (not error)
				{
					no = pthread_mutexattr_destroy(this);
				}
			}

			auto getprioceiling() const
			{
				int prioceiling;
				no = pthread_mutexattr_getprioceiling(this, &prioceiling);
				return prioceiling;
			}

			void setprioceiling(int prioceiling)
			{
				no = pthread_mutexattr_setprioceiling(this, prioceiling);
			}

			auto getprotocol() const
			{
				int protocol;
				no = pthread_mutexattr_getprotocol(this, &protocol);
				return protocol;
			}

			void setprotocol(int protocol)
			{
				no = pthread_mutexattr_setprotocol(this, protocol);
			}

			auto getpshared() const
			{
				int pshared;
				no = pthread_mutexattr_getpshared(this, &pshared);
				return pshared;
			}

			void setpshared(int pshared)
			{
				no = pthread_mutexattr_setpshared(this, pshared);
			}

			auto gettype() const
			{
				int type;
				no = pthread_mutexattr_gettype(this, &type);
				return type;
			}

			void settype(int type)
			{
				no = pthread_mutexattr_settype(this, type);
			}
		};
	};

	struct rwlock : pthread_rwlock_t
	{
		mutable int no = 0;

		rwlock(pthread_rwlock_attr const* attr = nullptr)
		{
			no = pthread_rwlock_init(this, attr);
		}

		~rwlock()
		{
			if (not error)
			{
				no = pthread_rwlock_destroy(this);
			}
		}

		void rdlock()
		{
			no = pthread_rwlock_rdlock(this);
		}

		void wrlock()
		{
			no = pthread_rwlock_wrlock(this);
		}

		void tryrdlock()
		{
			no = pthread_rwlock_tryrdlock(this);
		}

		void trywrlock()
		{
			no = pthread_rwlock_trywrlock(this);
		}

		void unlock()
		{
			no = pthread_rwlock_unlock(this);
		}

		struct attr : pthread_rwlockattr_t
		{
			rwlock init() const
			{
				return rwlock(this);
			}

			attr()
			{
				no = pthread_rwlockattr_init(this);
			}

			~attr()
			{
				if (not error)
				{
					no = pthread_rwlockattr_destroy(this);
				}
			}

			auto getpshared() const
			{
				int pshared;
				no = pthread_rwlockattr_getpshared(this, &pshared);
				return pshared;
			}

			void setpshared(int pshared)
			{
				no = pthread_rwlockattr_setpshared(this, pshared);
			}
		};
	};
}

#endif // file
