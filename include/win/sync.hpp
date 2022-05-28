#ifndef win_sync_hpp
#define win_sync_hpp "WIN32 Synchronization"

#include "win.hpp"
#include "ptr.hpp"
#include "sys.hpp"
#include "msg.hpp"
#include "profile.hpp"
#include <winbase.h>

namespace sys::win
{
	struct start : handle
	{
		unsigned id;

		start(fwd::event f, LPSECURITY_ATTRIBUTES attr = nullptr) : work(f)
		{
			auto const ptr = _beginthreadex(attr, 0, thread, this, 0, &id);
			h = reinterpret_cast<HANDLE>(ptr);
			if (fail(h))
			{
				win::err(here, "_beginthreadex");
			}
		}

	private:

		event work;

		static unsigned WINAPI thread(void *ptr)
		{
			auto that = fwd::cast_as<start>(ptr);
			#ifdef assert
			assert(nullptr != that);
			#endif
			if (that) that->work();
			_endthreadex(that->id);
			return that->id;
		}
	};

	struct join : start
	{
		~join()
		{
			if (wait(h))
			{
				warn(here, id);
			}
		}
	};

	struct security : size<&SECURITY_ATTRIBUTES::nLength>
	{
		security(bool inherit = true)
		{
			bInheritHandle = inherit ? TRUE : FALSE;
		}

		auto thread(start::function f)
		{
			return start(f, this);
		}

		auto pipes()
		{
			return pipe(this);
		}

		handle mutex(LPCSTR name, bool owner = false)
		{
			return CreateMutex(this, owner, name);
		}

		handle semaphore(LPCSTR name, long size = 8, long init = 0)
		{
			return CreateSemaphore(this, init, size, name);
		}

		handle event(LPCSTR name, bool init = false, bool manual = false)
		{
			return CreateEvent(this, manual, init, name);
		}

		handle job(LPCSTR name)
		{
			return CreateJobObject(this, name);
		}

		handle timer(LPCSTR name = nullptr, bool manual = false)
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

	struct timer : handle
	{
		timer(HANDLE h, fwd::function f, long long t, long p = 0, bool resume = true) : work(f), handle(h)
		{
			const large_int large = t;
			if (not SetWaitableTimer(h, large.buf, p, thread, this, resume))
			{
				sys::win::err(here, "SetWaitableTimer");
			}
		}

		~timer()
		{
			if (not sys::win::fail(h))
			{
				if (not CancelWaitableTimer(h))
				{
					sys::win::err(here, "CancelWaitableTimer");
				}
			}
		}

		static auto convert(std::timespec tv)
		{
			const auto div = std::div(tv.tv_nsec, 1e2L);
			std::lldiv_t ll;
			ll.quot = std::fma(tv.tv_sec, 1e7L, div.quot);
			ll.rem = div.rem;
			return ll;
		}

	private:

		function work;

		static void CALLBACK thread(LPVOID lp, DWORD low, DWORD high)
		{
			if (auto that = fwd::cast_as<timer>(lp); that)
			{
				that->work();
			}
		}
	};
}

namespace sys
{
	using thread = win::start;
	using join = win::join;

	struct mutex : win::critical_section
	{
		auto lock()
		{
			class unlock : fwd::no_copy
			{
				critical_section* that;

			public:

				unlock(critical_section* ptr) : that(ptr)
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

	struct rwlock : win::srwlock
	{
		auto reader()
		{
			class unlock : fwd::no_copy
			{
				srwlock* that;

			public:

				unlock(srwlock* ptr) : that(ptr)
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

		auto writer()
		{
			class unlock : fwd::no_copy
			{
				srwlock* that;

			public:

				unlock(srwlock* ptr) : that(ptr)
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
