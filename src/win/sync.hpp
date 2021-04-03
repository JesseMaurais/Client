#ifndef win_sync_hpp
#define win_sync_hpp "WIN32 Synchronization"

#include "win.hpp"
#include "ptr.hpp"
#include "sys.hpp"
#include <functional>

namespace sys::win
{
	struct start : handle
	{
		using function = std::function<void()>;

		unsigned id;

		start(function f, LPSECURITY_ATTRIBUTES attr = nullptr) : work(f)
		{
			h = _beginthreadex(attr, 0, thunk, this, 0, &id);
			if (fail(h))
			{
				err(here, "_beginthreadex");
			}
		}

		~start()
		{
			if (wait(h))
			{
				warn(here, id);
			}
		}

	private:

		function work;

		static unsigned thunk(void *ptr)
		{
			auto that = fwd::cast_as<start>(ptr);
			if (that) that->work();
			_endthreadex(that->id);
			return that->id;
		}
	};

	struct thread : handle
	{
		thread(DWORD id, DWORD dw = THREAD_ALL_ACCESS)
		{
			h = OpenThread(dw, false, id);
			if (fail(h))
			{
				err(here, "OpenThread", id);
			}
		}
	};

	struct process : handle
	{
		process(DWORD id, DWORD dw = PROCESS_ALL_ACCESS)
		{
			h = OpenProcess(dw, false, id);
			if (fail(h))
			{
				err(here, "OpenProcess", id);
			}
		}
	};

	struct timer : handle
	{
		timer(LPSTR name = nullptr, DWORD dw = TIMER_ALL_ACCESS)
		{
			h = OpenWaitableTimer(dw, false, name);
			if (fail(h))
			{
				err(here, "OpenWaitableTimer", name);
			}
		}
	};

	struct file_map : handle
	{
		file_map(LPSTR name = nullptr, DWORD dw = PROCESS_ALL_ACCESS)
		{
			h = OpenFileMapping(dw, false, name);
			if (fail(h))
			{
				err(here, "OpenFileMapping", name);
			}
		}
	};

	struct event : handle
	{
		event(LPSTR name = nullptr, DWORD dw = EVENT_ALL_ACCESS)
		{
			h = OpenEvent(dw, false, name);
			if (fail(h))
			{
				err(here, "OpenEvent", name);
			}
		}
	};

	struct mutex : handle
	{
		mutex(LPSTR name = nullptr, DWORD dw = MUTEX_ALL_ACCESS)
		{
			h = OpenMutex(dw, false, name);
			if (fail(h))
			{
				err(here, "OpenMutex", name);
			}
		}
	};

	struct semaphore : handle
	{
		semaphore(LPSTR name = nullptr, DWORD dw = SEMAPHORE_ALL_ACCESS)
		{
			h = OpenSemaphore(dw, false, name);
			if (fail(h))
			{
				err(here, "OpenSemaphore", name);
			}
		}
	};

	struct security : size<&SECURITY_ATTRIBUTES::nLength>
	{
		security(bool inherit = true)
		{
			bInheritHandle = inherit ? TRUE : FALSE;
		}

		auto thread(start::function f) const
		{
			return start(f, this);
		}

		auto pipes() const
		{
			return pipe(this);
		}

		handle mutex(LPCSTR name, bool owner = false)
		{
			return CreateMutex(name, owner, this);
		}

		handle semaphore(LPCSTR name, long size = 8, long init = 0)
		{
			return CreateSempahore(this, init, size, name);
		}

		handle event(LPCSTR name, bool init = false, bool manual = false)
		{
			return CreateEvent(this, manual, init, name);
		}

		handle job(LPCSTR name)
		{
			return CreateJobObject(this, name);
		}

		handle timer(LPCSTR name, bool manual = false)
		{
			return CreateWaitableTimer(this, manual, name);
		}

		handle map(LPCSTR name, HANDLE h, int prot, size_t sz)
		{
			return CreateFileMapping(h, this, prot, HIWORD(sz), LOWORD(sz), name);
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
	using thread = sys::win::start;

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
}

#endif // file
