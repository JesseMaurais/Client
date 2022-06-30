#ifndef win_sync_hpp
#define win_sync_hpp "WIN32 Synchronization"

#include "win.hpp"
#include "ptr.hpp"
#include "sys.hpp"
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
			reset(reinterpret_cast<HANDLE>(ptr));
			if (auto h = get(); fail(h))
			{
				win::err(here, "_beginthreadex");
			}
		}

	private:

		fwd::event work;

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
			if (auto h = get(); wait(h))
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

		auto thread(fwd::event f)
		{
			return start(f, this);
		}

		auto pipes()
		{
			return pipe(this);
		}

		auto mutex(bool owner = false, LPCSTR name = nullptr)
		{
			return CreateMutex(this, owner, name);
		}

		auto semaphore(long init = 0, long size = 8, LPCSTR name = nullptr)
		{
			return CreateSemaphore(this, init, size, name);
		}

		auto event(bool manual = false, bool init = false, LPCSTR name = nullptr)
		{
			return CreateEvent(this, manual, init, name);
		}

		auto job(LPCSTR name = nullptr)
		{
			return CreateJobObject(this, name);
		}

		auto timer(bool manual = false, LPCSTR name = nullptr)
		{
			return CreateWaitableTimer(this, manual, name);
		}

		auto map(HANDLE h, int prot, size_t sz, LPCSTR name = nullptr)
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
		timer(HANDLE h, fwd::event f, long long t, long p = 0, bool resume = true) : work(f), handle(h)
		{
			const large_int large = t;
			if (not SetWaitableTimer(h, large.buf, p, thread, this, resume))
			{
				sys::win::err(here, "SetWaitableTimer");
			}
		}

		~timer()
		{
			if (auto h = get(); not sys::win::fail(h))
			{
				if (not CancelWaitableTimer(h))
				{
					sys::win::err(here, "CancelWaitableTimer");
				}
			}
		}

		static auto convert(std::timespec tv)
		{
			const auto div = std::div(tv.tv_nsec, 100L);
			std::lldiv_t ll;
			ll.quot = tv.tv_sec * 10000000L + div.quot;
			ll.rem = div.rem;
			return ll;
		}

	private:

		fwd::event work;

		static void CALLBACK thread(LPVOID lp, [[maybe_unused]] DWORD low, [[maybe_unused]] DWORD high)
		{
			if (auto that = fwd::cast_as<timer>(lp); that)
			{
				that->work();
			}
		}
	};
}

#endif // file
