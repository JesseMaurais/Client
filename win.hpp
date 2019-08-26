#ifndef win_hpp
#define win_hpp

#ifndef _WIN32
# error Win32 utility header included without feature macro.
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include "sys.hpp"
#include "ptr.hpp"
#include "err.hpp"
#include "str.hpp"

#pragma comment(lib, "user32.lib")

namespace sys::win
{
	inline auto invalid = INVALID_HANDLE_VALUE;

	inline bool fail(HANDLE h)
	{
		return nullptr == h or invalid == h;
	}

	template <typename... Args>
	inline void err(Args... args)
	{
		if (sys::debug)
		{
			auto const no = GetLastError();
			auto const s = sys::win::fmt::err(no);
			sys::warn(args..., s);
		}
	}

	inline HANDLE get(int fd)
	{
		if (sys::fail(fd)) return sys::win::invalid;
		auto const iptr = _get_osfhandle(fd);
		return reinterpret_cast<HANDLE>(iptr);
	}

	inline int open(HANDLE h, int flags)
	{
		if (sys::win::fail(h)) return sys::invalid;
		auto const iptr = reinterpret_cast<intptr_t>(h);
		return _open_osfhandle(iptr, flags);
	}

	inline bool wait(HANDLE h, DWORD ms = INFINITE)
	{
		auto const dw = WaitForSingleObject(h, ms);
		if (WAIT_FAILED == dw)
		{
			sys::win::err(here, "WaitForSingleObject", ms);
			return failure;
		}
		return success;
	}

	struct msg : MSG
	{
		template <UINT Min, UINT Max> struct range : ::fmt::range<UINT>
		{
			using base = ::fmt::range<UINT>;
			range() : base(Min, Max) { }
		};

		using key   = range<WM_KEYFIRST, WM_KEYLAST>;
		using mouse = range<WM_MOUSEFIRST, WM_MOUSELAST>;
		using user  = range<WM_USER, 0x7FFF>;
		using app   = range<WM_APP, 0xBFFF>;
		using text  = range<0xC000, 0xFFFF>;

		inline bool get(UINT min = 0, UINT max = 0, HWND hw = nullptr)
		{
			auto const result = GetMessage(this, hw, min, max);
			if (sys::fail(result))
			{
				sys::win::err(here, "GetMessage");
				return failure;
			}
			return success;
		}

		static bool put(DWORD the, UINT msg, WPARAM wp = 0, LPARAM lp = 0)
		{
			if (not PostThreadMessage(the, msg, wp, lp))
			{
				sys::win::err
				(
					here, "PostThreadMessage", the, msg, wp, lp
				);
				return failure;
			}
			return success;
		}

		inline bool put(DWORD thread)
		{
			return put(thread, message, wParam, lParam);
		}
		
		static bool quit(DWORD thread)
		{
			return put(thread, WM_QUIT);
		}
	};

	template <class T> struct zero : T
	{
		zero() { ZeroMemory(this, sizeof(T)); }
	};

	template <class T, DWORD T::*Size> struct size : zero<T>
	{
		size() { this->*Size = sizeof(T); }
	};

	using security_attributes = size<SECURITY_ATTRIBUTES, &SECURITY_ATTRIBUTES::nLength>;
	using startup_info = size<STARTUPINFO, &STARTUPINFO::cb>;
	using process_info = zero<PROCESS_INFORMATION>;
	using process_entry = size<PROCESSENTRY32, &PROCESSENTRY32::dwSize>;
	using module_entry = size<MODULEENTRY32, &MODULEENTRY32::dwSize>;
	using thread_entry = size<THREADENTRY32, &THREADENTRY32::dwSize>;

	struct info : SYSTEM_INFO
	{
		info()
		{
			GetSystemInfo(this);
		}
	};

	struct handle : unique
	{
		HANDLE h;

		operator HANDLE() const
		{
			return h;
		}

		handle(HANDLE p = sys::win::invalid)
		: h(p)
		{ }

		~handle()
		{
			if (not sys::win::fail(h))
			{
				if (not CloseHandle(h))
				{
					sys::win::err(here, "CloseHandle");
				}
			}
		}

		int open(int flags)
		{
			int const fd = sys::win::open(h, flags);
			h = sys::win::invalid;
			return fd;
		}
	};

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

	struct pipe
	{
		handle read;
		handle write;

		pipe()
		{
			security_attributes sa;
			sa.bInheritHandle = true;
			if (not CreatePipe(&read.h, &write.h, &sa, BUFSIZ))
			{
				sys::win::err(here, "CreatePipe");
			}
		}
	};

	struct snapshot : handle
	{
		snapshot(DWORD dw)
		{
			h = CreateToolhelp32Snapshot(dw, 0);
			if (sys::win::fail(h))
			{
				sys::win::err(here, "CreateToolhelp32Snapshot", dw);
			}
		}
	};

	struct processes : process_entry
	{
		snapshot snap;

		processes() : snap(TH32CS_SNAPPROCESS)
		{
			if (not sys::win::fail(snap.h))
			{
				if (not Process32First(snap.h, this))
				{
					sys::win::err(here, "Process32First");
				}
			}
		}

		bool operator++()
		{
			return Process32Next(snap.h, this);
		}
	};

	struct modules : module_entry
	{
		snapshot snap;

		modules() : snap(TH32CS_SNAPMODULE)
		{
			if (not sys::win::fail(snap.h))
			{
				if (not Module32First(snap.h, this))
				{
					sys::win::err(here, "Module32First");
				}
			}
		}

		bool operator++()
		{
			return Module32Next(snap.h, this);
		}
	};

	struct threads : thread_entry
	{
		snapshot snap;

		threads() : snap(TH32CS_SNAPTHREAD)
		{
			if (not sys::win::fail(snap.h))
			{
				if (not Thread32First(snap.h, this))
				{
					sys::win::err(here, "Thread32First");
				}
			}
		}

		bool operator++()
		{
			return Thread32Next(snap.h, this);
		}
	};

	struct files : WIN32_FIND_DATA
	{
		HANDLE h;

		files(char const* s)
		{
			h = FindFirstFile(s, this);
			if (sys::win::fail(h))
			{
				sys::win::err(here, "FindFirstFile", s);
			}
		}

		~files()
		{
			if (not sys::win::fail(h))
			{
				if (not FindClose(h))
				{
					sys::win::err(here, "FindClose");
				}
			}
		}

		bool operator++()
		{
			if (not FindNextFile(h, this))
			{
				if (GetLastError() != ERROR_NO_MORE_FILES)
				{
					sys::win::err(here, "FindNextFile");
				}
				return false;
			}
			return true;
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
