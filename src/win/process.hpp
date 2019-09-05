#ifndef win_process_hpp
#define win_process_hpp

#include "win.hpp"
#include "sys.hpp"
#include "ptr.hpp"
#include "err.hpp"
#include "str.hpp"

namespace sys::win
{
	extern "C"
	{
		#include <tlhelp32.h>
	}

	struct snapshot : handle
	{
		snapshot(DWORD dw)
		{
			h = CreateToolhelp32Snapshot(dw, 0);
			if (sys::win::fail(h))
			{
				sys::win::err(here, dw);
			}
		}
	};

	struct process_entry : size<PROCESSENTRY32>
	{
		snapshot snap;

		process_entry() : snap(TH32CS_SNAPPROCESS)
		{
			if (not sys::win::fail(snap.h))
			{
				if (not Process32First(snap.h, this))
				{
					sys::win::err(here);
				}
			}
		}

		bool next()
		{
			return Process32Next(snap.h, this);
		}
	};

	struct module_entry : size<MODULEENTRY32>
	{
		snapshot snap;

		module_entry() : snap(TH32CS_SNAPMODULE)
		{
			if (not sys::win::fail(snap.h))
			{
				if (not Module32First(snap.h, this))
				{
					sys::win::err(here);
				}
			}
		}

		bool next()
		{
			return Module32Next(snap.h, this);
		}
	};

	struct thread_entry : size<THREADENTRY32>
	{
		snapshot snap;

		thread_entry() : snap(TH32CS_SNAPMODULE)
		{
			if (not sys::win::fail(snap.h))
			{
				if (not Thread32First(snap.h, this))
				{
					sys::win::err(here);
				}
			}
		}

		bool next()
		{
			return Thread32Next(snap.h, this);
		}
	};

	template <typename T> class iterable : T
	{
		using base = typename T;

		class iterator
		{
			iterable* that;
			bool flag;

		public:

			iterator(iterable* ptr, bool end)
			: that(ptr), flag(end)
			{ }

			bool operator!=(iterator const &it) const
			{
				return it.that != that or it.flag != flag;
			}

			auto& operator*() const
			{
				return *that;
			}

			auto& operator++()
			{
				flag = that->next();
				return *this;
			}
		};

	public:

		using base::base;

		auto begin()
		{
			return iterator(this, fail(base::h));
		}

		auto end()
		{
			return iterator(this, true);
		}
	};

	using processes = iterable<process_entry>;
	using modules = iterable<module_entry>;
	using threads = iterable<thread_entry>;
}

#endif // file
