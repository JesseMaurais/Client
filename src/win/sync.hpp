#ifndef win_thread_hpp
#define win_thread_hpp

#include "win.hpp"
#include "sys.hpp"

namespace sys::win
{
	struct process : handle
	{
		process(DWORD pid, DWORD dw = PROCESS_ALL_ACCESS)
		{
			h = OpenProcess(dw, false, pid);
			if (sys::win::fail(h))
			{
				sys::win::err(here, "OpenProcess", pid);
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

	struct event : handle
	{
		event(char const *name = nullptr, DWORD dw = EVENT_ALL_ACCESS)
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
		mutex(char const *name = nullptr, DWORD dw = MUTEX_ALL_ACCESS)
		{
			h = OpenMutex(dw, false, name);
			if (sys::win::fail(h))
			{
				sys::win::err(here, "OpenMutex", name);
			}
		}
	};

	struct critical : CRITICAL_SECTION
	{
		critical()
		{
			InitializeCriticalSection(this);
		}

		critical(DWORD spin_count)
		{
			InitializeCriticalSectionAndSpinCount(this, spin_count);
		}

		~critical()
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
	struct mutex : sys::win::handle
	{
		mutex()
		{
			h = CreateMutex(nullptr, true, nullptr);
			if (sys::win::fail(h))
			{
				sys::win::err(here, "CreateMutex");
			}
		}

		auto lock()
		{
			class unlock : unique
			{
				HANDLE h;

			public:

				unlock(HANDLE mutex) : h(mutex)
				{
					if (fail(sys::win::wait(h)))
					{
						sys::warn(here);
					}
				}

				~unlock()
				{
					if (not sys::win::fail(h))
					{
						if (not ReleaseMutex(h))
						{
							sys::win::err(here, "ReleaseMutex");
						}
					}
				}

			};
			return unlock(h);
		}
	};

	struct rwlock : sys::win::srwlock
	{
		auto read()
		{
			class unlock : unique
			{
				sys::win::srwlock* ptr;

			public:

				unlock(rwlock* that) : ptr(that)
				{
					ptr->lock();
				}

				~unlock()
				{
					ptr->unlock();
				}
			};
			return unlock(this);
		}

		auto write()
		{
			class unlock : unique
			{
				sys::win::srwlock* ptr;

			public:

				unlock(rwlock* that) : ptr(that)
				{
					ptr->xlock();
				}

				~unlock()
				{
					ptr->xunlock();
				}
			};
			return unlock(this);
		}
	};

	template <typename Routine>
	struct thread : sys::win::handle
	{
		DWORD id;

		using base = sys::win::handle;

		thread(Routine start) : work(start)
		{
			base::h = CreateThread(nullptr, 0, thunk, this, 0, &id);
			if (sys::win::fail(base::h))
			{
				sys::win::err(here, "CreateThread");
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

		static DWORD thunk(LPVOID ptr)
		{
			auto that = reinterpret_cast<thread>(ptr);
			that->work();
			return 0;
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
};

#endif // file
