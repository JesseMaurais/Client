#ifndef uni_hpp
#define uni_hpp

#ifdef _WIN32
# error POSIX utility header included with Win32 macro.
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

	namespace fmt
	{
		char* err(int no);
	}

	using routine = void*(void*);

	struct thread
	{
		pthread_attr_t self;
		mutable int no = 0;
		
		pthread_t create(routine* start, void* arg = nullptr) const
		{
			pthread_t id = sys::invalid;
			no = pthread_create(&id, &self, start, arg);
			if (no) sys::warn(here, fmt::err(no));
			return id;
		}

		thread()
		{
			no = pthread_attr_init(&self);
			if (no) sys::warn(here, fmt::err(no));
		}

		~thread()
		{
			if (not no)
			{
				no = pthread_attr_destroy(&self);
				if (no) sys::warn(here, fmt::err(no));
			}
		}

		auto getdetachstate() const
		{
			int state;
			no = pthread_attr_getdetachstate(&self, &state);
			if (no) sys::warn(here, fmt::err(no));
			return state;
		}

		void setdetachstate(int state)
		{
			no = pthread_attr_setdetachstate(&self, state);
			if (no) sys::warn(here, fmt::err(no));
		}

		auto getguardsize() const
		{
			size_t size;
			no = pthread_attr_getguardsize(&self, &size);
			if (no) sys::warn(here, fmt::err(no));
			return size;
		}

		void setguardsize(size_t size)
		{
			no = pthread_attr_setguardsize(&self, size);
			if (no) sys::warn(here, fmt::err(no));
		}

		auto getscope() const
		{
			int scope;
			no = pthread_attr_getscope(&self, &scope);
			if (no) sys::warn(here, fmt::err(no));
			return scope;
		}

		void setscope(int scope)
		{
			no = pthread_attr_setscope(&self, scope);
			if (no) sys::warn(here, fmt::err(no));
		}

		auto getstacksize() const
		{
			size_t size;
			no = pthread_attr_getstacksize(&self, &size);
			if (no) sys::warn(here, fmt::err(no));
			return size;
		}

		void setstacksize(size_t size)
		{
			no = pthread_attr_setstacksize(&self, size);
			if (no) sys::warn(here, fmt::err(no));
		}
	};

	struct cond
	{
		pthread_cond_t self;
		mutable int no = 0;

		cond(pthread_condattr_t const* attr = nullptr)
		{
			no = pthread_cond_init(&self, attr);
			if (no) sys::warn(here, fmt::err(no));
		}

		~cond()
		{
			if (not no)
			{
				no = pthread_cond_destroy(&self);
				if (no) sys::warn(here, fmt::err(no));
			}
		}

		void wait(pthread_mutex_t* mutex = nullptr)
		{
			no = pthread_cond_wait(&self, mutex);
			if (no) sys::warn(here, fmt::err(no));
		}

		void signal()
		{
			no = pthread_cond_signal(&self);
			if (no) sys::warn(here, fmt::err(no));
		}

		void broadcast()
		{
			no = pthread_cond_broadcast(&self);
			if (no) sys::warn(here, fmt::err(no));
		}

		struct attr
		{
			pthread_condattr_t self;
			mutable int no = 0;

			cond init() const
			{
				return cond(&self);
			}

			attr()
			{
				no = pthread_condattr_init(&self);
				if (no) sys::warn(here, fmt::err(no));
			}

			~attr()
			{
				if (not no)
				{
					no = pthread_condattr_destroy(&self);
					if (no) sys::warn(here, fmt::err(no));
				}
			}

			auto getpshared() const
			{
				int pshared;
				no = pthread_condattr_getpshared(&self, &pshared);
				if (no) sys::warn(here, fmt::err(no));
				return pshared;
			}

			void setpshared(int pshared)
			{
				no = pthread_condattr_setpshared(&self, pshared);
				if (no) sys::warn(here, fmt::err(no));
			}
		};
	};

	struct mutex
	{
		pthread_mutex_t self;
		mutable int no = 0;

		mutex(pthread_mutexattr_t const* attr = nullptr)
		{
			no = pthread_mutex_init(&self, attr);
			if (no) sys::warn(here, fmt::err(no));
		}

		~mutex()
		{
			if (not no)
			{
				no = pthread_mutex_destroy(&self);
				if (no) sys::warn(here, fmt::err(no));
			}
		}

		auto getprioceiling() const
		{
			int prioceiling;
			no = pthread_mutex_getprioceiling(&self, &prioceiling);
			if (no) sys::warn(here, fmt::err(no));
			return prioceiling;
		}

		auto setprioceiling(int prioceiling)
		{
			no = pthread_mutex_setprioceiling(&self, prioceiling, &prioceiling);
			if (no) sys::warn(here, fmt::err(no));
			return prioceiling;
		}

		void lock()
		{
			no = pthread_mutex_lock(&self);
			if (no) sys::warn(here, fmt::err(no));
		}

		void unlock()
		{
			no = pthread_mutex_lock(&self);
			if (no) sys::warn(here, fmt::err(no));
		}

		void trylock()
		{
			no = pthread_mutex_trylock(&self);
			if (no) sys::warn(here, fmt::err(no));
		}

		struct attr
		{
			pthread_mutexattr_t self;
			mutable int no = 0;

			mutex init() const
			{
				return mutex(&self);
			}

			attr()
			{
				no = pthread_mutexattr_init(&self);
				if (no) sys::warn(here, fmt::err(no));
			}

			~attr()
			{
				if (not no)
				{
					no = pthread_mutexattr_destroy(&self);
					if (no) sys::warn(here, fmt::err(no));
				}
			}

			auto getprioceiling() const
			{
				int prioceiling;
				no = pthread_mutexattr_getprioceiling(&self, &prioceiling);
				if (no) sys::warn(here, fmt::err(no));
				return prioceiling;
			}

			void setprioceiling(int prioceiling)
			{
				no = pthread_mutexattr_setprioceiling(&self, prioceiling);
				if (no) sys::warn(here, fmt::err(no));
			}

			auto getprotocol() const
			{
				int protocol;
				no = pthread_mutexattr_getprotocol(&self, &protocol);
				if (no) sys::warn(here, fmt::err(no));
				return protocol;
			}

			void setprotocol(int protocol)
			{
				no = pthread_mutexattr_setprotocol(&self, protocol);
				if (no) sys::warn(here, fmt::err(no));
			}

			auto getpshared() const
			{
				int pshared;
				no = pthread_mutexattr_getpshared(&self, &pshared);
				if (no) sys::warn(here, fmt::err(no));
				return pshared;
			}

			void setpshared(int pshared)
			{
				no = pthread_mutexattr_setpshared(&self, pshared);
				if (no) sys::warn(here, fmt::err(no));
			}

			auto gettype() const
			{
				int type;
				no = pthread_mutexattr_gettype(&self, &type);
				if (no) sys::warn(here, fmt::err(no));
				return type;
			}

			void settype(int type)
			{
				no = pthread_mutexattr_settype(&self, type);
				if (no) sys::warn(here, fmt::err(no));
			}
		};
	};

	struct rwlock
	{
		pthread_rwlock_t self;
		mutable int no = 0;

		rwlock(pthread_rwlockattr_t const* attr = nullptr)
		{
			no = pthread_rwlock_init(&self, attr);
			if (no) sys::warn(here, fmt::err(no));
		}

		~rwlock()
		{
			if (not no)
			{
				no = pthread_rwlock_destroy(&self);
				if (no) sys::warn(here, fmt::err(no));
			}
		}

		void rdlock()
		{
			no = pthread_rwlock_rdlock(&self);
			if (no) sys::warn(here, fmt::err(no));
		}

		void wrlock()
		{
			no = pthread_rwlock_wrlock(&self);
			if (no) sys::warn(here, fmt::err(no));
		}

		void tryrdlock()
		{
			no = pthread_rwlock_tryrdlock(&self);
			if (no) sys::warn(here, fmt::err(no));
		}

		void trywrlock()
		{
			no = pthread_rwlock_trywrlock(&self);
			if (no) sys::warn(here, fmt::err(no));
		}

		void unlock()
		{
			no = pthread_rwlock_unlock(&self);
			if (no) sys::warn(here, fmt::err(no));
		}

		struct attr
		{
			pthread_rwlockattr_t self;
			mutable int no = 0;

			rwlock init() const
			{
				return rwlock(&self);
			}

			attr()
			{
				no = pthread_rwlockattr_init(&self);
				if (no) sys::warn(here, fmt::err(no));
			}

			~attr()
			{
				if (not no)
				{
					no = pthread_rwlockattr_destroy(&self);
					if (no) sys::warn(here, fmt::err(no));
				}
			}

			auto getpshared() const
			{
				int pshared;
				no = pthread_rwlockattr_getpshared(&self, &pshared);
				if (no) sys::warn(here, fmt::err(no));
				return pshared;
			}

			void setpshared(int pshared)
			{
				no = pthread_rwlockattr_setpshared(&self, pshared);
				if (no) sys::warn(here, fmt::err(no));
			}
		};
	};
}

#endif // file
