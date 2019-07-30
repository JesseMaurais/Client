#ifndef uni_hpp
#define uni_hpp

#ifdef _WIN32
# error POSIX utility header included with Win32 macro.
#endif

#include <pthread.h>
#include <dirent.h>
#include "bool.hpp"
#include "sys.hpp"
#include "err.hpp"

namespace sys::uni
{
	namespace fmt
	{
		char* err(int no);
	}

	template <typename... Args>
	inline void err(int no, Args... args)
	{
		sys::warn(args..., fmt::err(no));
	}

	struct files
	{
		DIR* ptr;

		files(char const *s)
		{
			ptr = opendir(s);
			if (ptr)
			{
				sys::err(here, "opendir", s);
			}
		}

		~files()
		{
			if (ptr and fail(closedir(ptr)))
			{
				sys::err(here, "closedir");
			}
		}

		auto operator++()
		{
			return readdir(ptr);
		}
	};

	using routine = void*(void*);

	struct thread
	{
		pthread_attr_t self;
		mutable int no = 0;
		
		pthread_t create(routine* start, void* arg = nullptr) const
		{
			pthread_t id;
			no = pthread_create(&id, &self, start, arg);
			if (no) sys::uni::err(no, here);
			return id;
		}

		thread()
		{
			no = pthread_attr_init(&self);
			if (no) sys::uni::err(no, here);
		}

		~thread()
		{
			if (not no)
			{
				no = pthread_attr_destroy(&self);
				if (no) sys::uni::err(no, here);
			}
		}

		auto getdetachstate() const
		{
			int state;
			no = pthread_attr_getdetachstate(&self, &state);
			if (no) sys::uni::err(no, here);
			return state;
		}

		void setdetachstate(int state)
		{
			no = pthread_attr_setdetachstate(&self, state);
			if (no) sys::uni::err(no, here);
		}

		auto getguardsize() const
		{
			size_t size;
			no = pthread_attr_getguardsize(&self, &size);
			if (no) sys::uni::err(no, here);
			return size;
		}

		void setguardsize(size_t size)
		{
			no = pthread_attr_setguardsize(&self, size);
			if (no) sys::uni::err(no, here);
		}

		auto getscope() const
		{
			int scope;
			no = pthread_attr_getscope(&self, &scope);
			if (no) sys::uni::err(no, here);
			return scope;
		}

		void setscope(int scope)
		{
			no = pthread_attr_setscope(&self, scope);
			if (no) sys::uni::err(no, here);
		}

		auto getstacksize() const
		{
			size_t size;
			no = pthread_attr_getstacksize(&self, &size);
			if (no) sys::uni::err(no, here);
			return size;
		}

		void setstacksize(size_t size)
		{
			no = pthread_attr_setstacksize(&self, size);
			if (no) sys::uni::err(no, here);
		}
	};

	struct cond
	{
		pthread_cond_t self;
		mutable int no = 0;

		cond(pthread_condattr_t const* attr = nullptr)
		{
			no = pthread_cond_init(&self, attr);
			if (no) sys::uni::err(no, here);
		}

		~cond()
		{
			if (not no)
			{
				no = pthread_cond_destroy(&self);
				if (no) sys::uni::err(no, here);
			}
		}

		void wait(pthread_mutex_t* mutex)
		{
			no = pthread_cond_wait(&self, mutex);
			if (no) sys::uni::err(no, here);
		}

		void signal()
		{
			no = pthread_cond_signal(&self);
			if (no) sys::uni::err(no, here);
		}

		void broadcast()
		{
			no = pthread_cond_broadcast(&self);
			if (no) sys::uni::err(no, here);
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
				if (no) sys::uni::err(no, here);
			}

			~attr()
			{
				if (not no)
				{
					no = pthread_condattr_destroy(&self);
					if (no) sys::uni::err(no, here);
				}
			}

			auto getpshared() const
			{
				int pshared;
				no = pthread_condattr_getpshared(&self, &pshared);
				if (no) sys::uni::err(no, here);
				return pshared;
			}

			void setpshared(int pshared)
			{
				no = pthread_condattr_setpshared(&self, pshared);
				if (no) sys::uni::err(no, here);
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
			if (no) sys::uni::err(no, here);
		}

		~mutex()
		{
			if (not no)
			{
				no = pthread_mutex_destroy(&self);
				if (no) sys::uni::err(no, here);
			}
		}

		auto getprioceiling() const
		{
			int prioceiling;
			no = pthread_mutex_getprioceiling(&self, &prioceiling);
			if (no) sys::uni::err(no, here);
			return prioceiling;
		}

		auto setprioceiling(int prioceiling)
		{
			no = pthread_mutex_setprioceiling(&self, prioceiling, &prioceiling);
			if (no) sys::uni::err(no, here);
			return prioceiling;
		}

		void lock()
		{
			no = pthread_mutex_lock(&self);
			if (no) sys::uni::err(no, here);
		}

		void unlock()
		{
			no = pthread_mutex_unlock(&self);
			if (no) sys::uni::err(no, here);
		}

		void trylock()
		{
			no = pthread_mutex_trylock(&self);
			if (no) sys::uni::err(no, here);
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
				if (no) sys::uni::err(no, here);
			}

			~attr()
			{
				if (not no)
				{
					no = pthread_mutexattr_destroy(&self);
					if (no) sys::uni::err(no, here);
				}
			}

			auto getprioceiling() const
			{
				int prioceiling;
				no = pthread_mutexattr_getprioceiling(&self, &prioceiling);
				if (no) sys::uni::err(no, here);
				return prioceiling;
			}

			void setprioceiling(int prioceiling)
			{
				no = pthread_mutexattr_setprioceiling(&self, prioceiling);
				if (no) sys::uni::err(no, here);
			}

			auto getprotocol() const
			{
				int protocol;
				no = pthread_mutexattr_getprotocol(&self, &protocol);
				if (no) sys::uni::err(no, here);
				return protocol;
			}

			void setprotocol(int protocol)
			{
				no = pthread_mutexattr_setprotocol(&self, protocol);
				if (no) sys::uni::err(no, here);
			}

			auto getpshared() const
			{
				int pshared;
				no = pthread_mutexattr_getpshared(&self, &pshared);
				if (no) sys::uni::err(no, here);
				return pshared;
			}

			void setpshared(int pshared)
			{
				no = pthread_mutexattr_setpshared(&self, pshared);
				if (no) sys::uni::err(no, here);
			}

			auto gettype() const
			{
				int type;
				no = pthread_mutexattr_gettype(&self, &type);
				if (no) sys::uni::err(no, here);
				return type;
			}

			void settype(int type)
			{
				no = pthread_mutexattr_settype(&self, type);
				if (no) sys::uni::err(no, here);
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
			if (no) sys::uni::err(no, here);
		}

		~rwlock()
		{
			if (not no)
			{
				no = pthread_rwlock_destroy(&self);
				if (no) sys::uni::err(no, here);
			}
		}

		void rdlock()
		{
			no = pthread_rwlock_rdlock(&self);
			if (no) sys::uni::err(no, here);
		}

		void wrlock()
		{
			no = pthread_rwlock_wrlock(&self);
			if (no) sys::uni::err(no, here);
		}

		void tryrdlock()
		{
			no = pthread_rwlock_tryrdlock(&self);
			if (no) sys::uni::err(no, here);
		}

		void trywrlock()
		{
			no = pthread_rwlock_trywrlock(&self);
			if (no) sys::uni::err(no, here);
		}

		void unlock()
		{
			no = pthread_rwlock_unlock(&self);
			if (no) sys::uni::err(no, here);
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
				if (no) sys::uni::err(no, here);
			}

			~attr()
			{
				if (not no)
				{
					no = pthread_rwlockattr_destroy(&self);
					if (no) sys::uni::err(no, here);
				}
			}

			auto getpshared() const
			{
				int pshared;
				no = pthread_rwlockattr_getpshared(&self, &pshared);
				if (no) sys::uni::err(no, here);
				return pshared;
			}

			void setpshared(int pshared)
			{
				no = pthread_rwlockattr_setpshared(&self, pshared);
				if (no) sys::uni::err(no, here);
			}
		};
	};
}

namespace sys
{
	struct mutex : sys::uni::mutex
	{
		auto lock()
		{
			struct key
			{
				sys::uni::mutex* ptr;

				key(mutex* that) : ptr(that)
				{
					ptr->lock();
				}

				~key()
				{
					ptr->unlock();
				}

			} unlock(this);
			return unlock;
		}
	};

	template <typename Routine>
	struct thread
	{
		pthread_t id;

		thread(Routine start) : work(start)
		{
			static sys::uni::thread attr;
			id = attr.create(thunk, this);
		}

		~thread()
		{
			void* ptr = nullptr;
			int no = pthread_join(id, &ptr);
			assert(this == ptr);
			if (fail(no))
			{
				sys::uni::err(no, here, id);
			}
		}

	private:

		Routine work;

		static void* thunk(void* ptr)
		{
			auto that = reinterpret_cast<thread>(ptr);
			that->work();
			return ptr;
		}
	};

	struct event : sys::uni::cond
	{
		using base = sys::uni::cond;

		bool wait(pthread_mutex_t& key)
		{
			base::wait(&key);
			return fail(no);
		}
	};
}

#endif // file
