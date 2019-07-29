#ifndef win_hpp
#define win_hpp

#ifndef _WIN32
# error Win32 utility header included without feature macro.
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include "sys.hpp"
#include "sig.hpp"
#include "err.hpp"

namespace sys::win
{
	constexpr auto invalid = INVALID_HANDLE_VALUE;

	inline bool fail(HANDLE h)
	{
		return nullptr == h or invalid == h;
	}

	namespace fmt
	{
		LPSTR err(DWORD id, HMODULE h = nullptr);
	}

	template <typename... Args>
	inline void err(Args... args)
	{
		if (sys::debug)
		{
			auto const no = GetLastError();
			auto const s = fmt::err(no);
			sys::warn(args.., s);
		}
	}

	inline HANDLE get(int fd)
	{
		if (sys::fail(fd)) return INVALID_HANDLE_VALUE;
		auto const iptr = _get_osfhandle(fd);
		return reinterpret_cast<HANDLE>(iptr);
	}

	inline int open(HANDLE h, int flags)
	{
		if (sys::win::fail(h)) return sys::invalid;
		auto const iptr = reinterpret_cast<intptr_t>(h);
		return _open_osfhandle(iptr, flags);
	}

	namespace sig
	{
		struct quit : link
		{
			quit(int on = SIGTERM) : link(on, PostQuitMessage) { }
		};
	}

	namespace msg
	{
		inline bool put(DWORD thid, UINT msg, WPARAM wp = 0, LPARAM lp = 0)
		{
			if (not PostThreadMessage(thid, msg, wp, lp)
			{
				sys::win::err(here, "PostThreadMessage", thid);
				return true;
			}
			return false;
		}
		
		inline bool quit(DWORD thid)
		{
			return put(thid, WM_QUIT);
		}
	}

	template <typename T> struct zero : T
	{
		zero() { ZeroMemory(this, sizeof(T)); }
	};

	template <typename T, DWORD T::*Size> struct size : zero<T>
	{
		size() { this->*Size = sizeof(T); }
	};

	using security_attributes = size<SECURITY_ATTRIBUTES, &SECURITY_ATTRIBUTES::nLength>;
	using startup_info = size<STARTUPINFO, &STARTUPINFO::cb>;
	using process_info = zero<PROCESS_INFORMATION>;
	using process_entry = size<PROCESSENTRY32, &PROCESSENTRY32::cwSize>;
	using module_entry = size<MODULEENTRY32, &MODULEENTRY32::cwSize>;
	using thread_entry = size<THREADENTRY32, &THREADENTRY32::cwSize>;

	struct handle
	{
		HANDLE h;

		handle(HANDLE p = invalid)
		: h(p)
		{ }

		~handle()
		{
			bool const ok = not sys::win::fail(h);
			if (ok and not CloseHandle(h))
			{
				sys::win::err(here, "CloseHandle");
			}
		}

		int open(int flags)
		{
			int const fd = open(h, flags);
			h = invalid;
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
		thread(DWORD thid, DWORD dw = THREAD_ALL_ACCESS)
		{
			h = OpenThread(dw, false, thid);
			if (sys::win::fail(h))
			{
				sys::win::err(here, "OpenThread", thid);
			}
		}
	};

	struct event : handle
	{
		event(char const *name, DWORD dw = EVENT_ALL_ACCESS)
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
		mutex(char const *name, DWORD dw = MUTEX_ALL_ACCESS)
		{
			h = OpenMutex(dw, false, name);
			if (sys::win::fail(h))
			{
				sys::win::err(here, "OpenMutex", name);
			}
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
		snapshot snap(TH32CS_SNAPPROCESS);

		processes()
		{
			if (not Process32First(h, this))
			{
				sys::win::err(here, "Process32First");
			}
		}

		bool operator++()
		{
			return Process32Next(h, this);
		}
	};

	struct modules : module_entry
	{
		snapshot snap(TH32CS_SNAPMODULE;

		modules()
		{
			if (not Module32First(h, this))
			{
				sys::win::err(here, "Module32First");
			}
		}

		bool operator++()
		{
			return Module32Next(h, this);
		}
	};

	struct threads : thread_entry
	{
		snapshot snap(TH32CS_SNAPTHREAD);

		threads()
		{
			if (not Thread32First(h, this))
			{
				sys::win::err(here, "Thread32First");
			}
		}

		bool operator++()
		{
			return Thread32Next(h, this);
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
			bool const ok = not sys::win::fail(h);
			if (ok and not FileClose(h))
			{
				sys::win::err(here, "FindClose");
			}
		}

		bool operator++()
		{
			if (not FindNextFile(h, this))
			{
				auto const e = GetLastError();
				if (ERROR_NO_MORE_FILES != e)
				{
					sys::win::err(here, "FindNextFile");
				}
				return false;
			}
			return true;
		}
	};
}

#endif // file
