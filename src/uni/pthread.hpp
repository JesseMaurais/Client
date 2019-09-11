#ifndef uni_thread_hpp
#define uni_thread_hpp

#include "uni.hpp"
#include "sys.hpp"
#include "ptr.hpp"
#include "err.hpp"
#include <pthread.h>

namespace sys::uni
{
	using routine = void*(void*);

	struct thread : unique
	{
		pthread_attr_t self;
		mutable int no = 0;
		
		pthread_t create(routine* start, void* that = nullptr) const
		{
			pthread_t id;
			no = pthread_create(&id, &self, start, that);
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

	struct cond : unique
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

	struct mutex : unique
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

	struct rwlock : unique
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
			class unlock : unique
			{
				sys::uni::mutex* that;

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

	struct rwlock : sys::uni::rwlock
	{
		auto read()
		{
			class unlock : unique
			{
				sys::uni::rwlock* that;

			public:

				unlock(sys::uni::rwlock* ptr) : that(ptr)
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
			class unlock : unique
			{
				sys::uni::rwlock* that;

			public:
			
				unlock(sys::uni::rwlock* ptr) : that(ptr)
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

	template <typename Routine>
	struct thread : unique
	{
		pthread_t id;

		thread(Routine start) : work(start)
		{
			static sys::uni::thread attr;
			id = attr.create(thunk, this);
		}

		~thread()
		{
			void* that = nullptr;
			int const no = pthread_join(id, &that);
			if (fail(no))
			{
				sys::uni::err(no, here, id);
			}
			else
			{
				assert(this == that);
			}
		}

	private:

		Routine work;

		static void* thunk(void* ptr)
		{
			auto that = reinterpret_cast<thread*>(ptr);
			that->work();
			return ptr;
		}
	};

	struct event : sys::uni::cond
	{
		bool wait(mutex& key)
		{
			cond::wait(&key.self);
			return fail(no);
		}
	};
}

#endif // file
