#ifndef win_hpp
#define win_hpp

#ifndef _WIN32
# error Win32 utility header included without feature macro.
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "sys.hpp"
#include "err.hpp"

namespace sys::win
{
	constexpr auto invalid = INVALID_HANDLE_VALUE;

	inline bool fail(HANDLE h)
	{
		return nullptr == h or invalid == h;
	}

	namespace msg
	{
		LPSTR err(HMODULE h = nullptr);

		HWND get(DWORD pid, DWORD& tid);
		HWND get(DWORD pid);

		inline bool put(UINT msg, HWND w = nullptr, WPARAM wp = 0, LPARAM lp = 0)
		{
			return PostMessage(w, msg, wp, lp);
		}

		inline bool get(MSG& it, UINT min = 0, UINT max = 0, HWND w = nullptr, UINT rm = PM_REMOVE)
		{
			return 0 != PeekMessage(&it, w, min, max, rm);
		}

		inline bool join(MSG& it)
		{
			return get(it) and WM_QUIT == it.message; 
		}

		inline HWND quit(DWORD pid)
		{
			auto const w = get(pid);
			if (not fail(w))
			{
				put(WM_QUIT, w);
			}
			return w;
		}
	}

	namespace sig
	{
		template <int On>
		struct quit : link
		{
			quit() : link(on, post) { }

		protected:

			void post(int on)
			{
				if (On == on)
				{
					PostQuitMessage(on);
				}
			}
		};

		using interrupt = quit<SIGINT>;
	}

	template <typename... Args>
	inline void err(Args... args)
	{
		if (sys::debug)
		{
			auto const s = msg::err();
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
		if (fail(h)) return sys::invalid;
		auto const iptr = reinterpret_cast<intptr_t>(h);
		return _open_osfhandle(iptr, flags);
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

	struct handle
	{
		HANDLE h;

		handle(HANDLE p = invalid)
		: h(p)
		{ }

		~handle()
		{
			if (not fail(h))
			{
				close();
			}
		}

		int open(int flags)
		{
			int const fd = open(h, flags);
			h = invalid;
			return fd;
		}

		void close()
		{
			if (not CloseHandle(h))
			{
				err(here, "CloseHandle");
			}
			else h = invalid;
		}
	};

	struct process : handle
	{
		PROCESS(DWORD pid)
		{
			h = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
			if (fail(h))
			{
				err(herr, "OpenProcess", pid);
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
				err(here, "CreatePipe");
			}
		}
	};

	inline bool fail(pipe const& p)
	{
		return fail(p.read) or fail(p.write);
	}

	struct find : WIN32_FIND_DATA
	{
		HANDLE h;

		find(char const* s)
		{
			h = FindFirstFile(s, this);
			if (fail(h))
			{
				err(here, "FindFirstFile", s);
			}
		}

		~find()
		{
			if (not fail(h))
			{
				close();
			}
		}

		bool operator++()
		{
			if (not FindNextFile(h, this))
			{
				auto const e = GetLastError();
				if (ERROR_NO_MORE_FILES != e)
				{
					err(here, "FindNextFile");
				}
				return false;
			}
			return true;
		}

		void close()
		{
			if (not FileClose(h))
			{
				err(here, "FindClose");
			}
			else h = invalid;
		}
	};

	inline bool fail(find const& f)
	{
		return fail(f.h);
	}

	struct process_iterator : process_entry
	{
		handle h;

		entry(DWORD dw = TH32CS_SNAPPROCESS)
		{
			h = CreateToolhelp32Snapshot(dw, 0);
			if (fail(h))
			{
				err(here, "CreateToolhelp32Snapshot", dw);
			}
			else
			if (not Process32First(h, this))
			{
				err(herr, "Process32First");
			}
		}

		bool operator++()
		{
			return Process32Next(h, this);
		}
	}

	inline bool fail(entry const& e)
	{
		return fail(e.h);
	}
}

#endif // file
