#ifndef win_sync_hpp
#define win_sync_hpp "WIN32 Synchronization"

#include "win.hpp"
#include "ptr.hpp"
#include "sys.hpp"

namespace sys::win
{
	struct process : handle
	{
		process(DWORD id, DWORD dw = PROCESS_ALL_ACCESS)
		{
			h = OpenProcess(dw, false, id);
			if (sys::win::fail(h))
			{
				sys::win::err(here, "OpenProcess", id);
			}
		}
	};

	struct thread : handle
	{
		thread(DWORD id, DWORD dw = THREAD_ALL_ACCESS)
		{
			h = OpenThread(dw, false, id);
			if (sys::win::fail(h))
			{
				sys::win::err(here, "OpenThread", id);
			}
		}
	};

	struct timer : handle
	{
		timer(LPSTR name = nullptr, DWORD dw = TIMER_ALL_ACCESS)
		{
			h = OpenWaitableTimer(dw, false, name);
			if (sys::win::fail(h))
			{
				sys::win::err(here, "OpenWaitableTimer", name);
			}
		}
	};

	struct file_map : handle
	{
		file_map(LPSTR name = nullptr, DWORD dw = PROCESS_ALL_ACCESS)
		{
			h = OpenFileMapping(dw, false, name);
			if (sys::win::fail(h))
			{
				sys::win::err(here, "OpenFileMapping", name);
			}
		}
	};

	struct event : handle
	{
		event(LPSTR name = nullptr, DWORD dw = EVENT_ALL_ACCESS)
		{
			h = OpenEvent(dw, false, name);
			if (sys::win::fail(h))
			{
				sys::win::err(here, "OpenEvent", name);
			}
		}
	};

	struct mutex : handle
	{
		mutex(LPSTR name = nullptr, DWORD dw = MUTEX_ALL_ACCESS)
		{
			h = OpenMutex(dw, false, name);
			if (sys::win::fail(h))
			{
				sys::win::err(here, "OpenMutex", name);
			}
		}
	};

	struct semaphore : handle
	{
		semaphore(LPSTR name = nullptr, DWORD dw = SEMAPHORE_ALL_ACCESS)
		{
			h = OpenSemaphore(dw, false, name);
			if (sys::win::fail(h))
			{
				sys::win::err(here, "OpenSemaphore", name);
			}
		}
	};

	struct critical_section : CRITICAL_SECTION
	{
		critical_section()
		{
			InitializeCriticalSection(this);
		}

		critical_section(DWORD spin_count)
		{
			InitializeCriticalSectionAndSpinCount(this, spin_count);
		}

		~critical_section()
		{
			DeleteCriticalSection(this);
		}

		auto set(DWORD spin_count)
		{
			return SetCriticalSectionSpinCount(this, spin_count);
		}

		void enter()
		{
			EnterCriticalSection(this);
		}

		bool tryenter()
		{
			return TRUE == TryEnterCriticalSection(this);
		}

		void leave()
		{
			LeaveCriticalSection(this);
		}
	};

	struct srwlock : SRWLOCK
	{
		srwlock()
		{
			InitializeSRWLock(this);
		}

		void lock()
		{
			AcquireSRWLockShared(this);
		}

		void xlock()
		{
			AcquireSRWLockExclusive(this);
		}

		void unlock()
		{
			ReleaseSRWLockShared(this);
		}

		void xunlock()
		{
			ReleaseSRWLockExclusive(this);
		}

		auto trylock()
		{
			return TryAcquireSRWLockShared(this);
		}

		auto xtrylock()
		{
			return TryAcquireSRWLockExclusive(this);
		}
	};
}

namespace sys
{
	struct mutex : sys::win::critical_section
	{
		auto lock()
		{
			class unlock : fwd::unique
			{
				sys::win::critical_section* that;

			public:

				unlock(sys::win::critical_section* ptr) : that(ptr)
				{
					that->enter();
				}

				~unlock()
				{
					that->leave();
				}

			};
			return unlock(this);
		}
	};

	struct rwlock : sys::win::srwlock
	{
		auto read()
		{
			class unlock : fwd::unique
			{
				sys::win::srwlock* that;

			public:

				unlock(sys::win::srwlock* ptr) : that(ptr)
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

		auto write()
		{
			class unlock : fwd::unique
			{
				sys::win::srwlock* that;

			public:

				unlock(sys::win::srwlock* ptr) : that(ptr)
				{
					that->xlock();
				}

				~unlock()
				{
					that->xunlock();
				}
			};
			return unlock(this);
		}
	};

	template <typename Routine> struct thread : sys::win::handle
	{
		unsigned id;

		thread(Routine start, LPSECURITY_ATTRIBUTES attr = nullptr) 
		: work(start)
		{
			auto const ptr = _beginthreadex(attr, 0, thunk, this, 0, &id);
			this->h = reinterpret_cast<HANDLE>(ptr);
			if (sys::win::fail(base::h))
			{
				sys::win::err(here);
			}
		}

		~thread()
		{
			if (fail(sys::win::wait(base::h)))
			{
				sys::warn(here, id);
			}
		}

	private:

		Routine work;

		static unsigned thunk(void *ptr)
		{
			auto that = reinterpret_cast<thread*>(ptr);
			that->work();
			_endthreadex(that->id);
			return that->id;
		}
	};

	struct event : sys::win::handle
	{
		event()
		{
			h = CreateEvent(nullptr, false, false, nullptr);
			if (sys::win::fail(h))
			{
				sys::win::err(here, "CreateEvent");
			}
		}

		bool signal()
		{
			if (not SetEvent(h))
			{
				sys::win::err(here, "SetEvent");
				return failure;
			}
			return success;
		}

		bool wait(mutex& key)
		{
			(void) key;
			if (fail(sys::win::wait(h)))
			{
				sys::warn(here);
				return failure;
			}
			return success;
		}
	};
}

namespace sys::win
{
	struct security : sys::win::size<&SECURITY_ATTRIBUTES::nLength>
	{
		security(bool inherit = true)
		{
			bInheritHandle = inherit ? TRUE : FALSE;
		}

		template <class Routine> auto thread(Routine work) const
		{
			return sys::win::thread(work, this);
		}

		auto pipe() const
		{
			return sys::win::pipe(this);
		}

		handle mutex(LPCSTR name = nullptr, bool owner = false)
		{
			return CreateMutex(name, owner, this);
		}

		handle semaphore(LPCSTR name = nullptr, long init = 0, long size = 1)
		{
			return CreateSempahore(this, init, size, name);
		}

		handle event(LPCSTR name = nullptr, bool init = false, bool manual = false)
		{
			return CreateEvent(this, manual, init, name);
		}

		handle job(LPCSTR name = nullptr)
		{
			return CreateJobObject(this, name);
		}

		handle timer(LPCSTR name = nullptr, bool manual = false)
		{
			return CreateWaitableTimer(this, manual, name);
		}
	};
};

#endif // file
