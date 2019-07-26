// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dll.hpp"
#include "sys.hpp"
#include "fmt.hpp"
#include "dir.hpp"
#include "err.hpp"

#ifdef _WIN32
# include "win.hpp"
#else
# include <dlfcn.h>
#endif

namespace
{
	#ifndef _WIN32
	template <typename... Args>
	inline void err(Args... args)
	{
		if (sys::debug)
		{
			auto const s = fmt::err(args...);
			std::fputs(data(s), stderr);
		}
	}
	#endif
}

namespace sys
{
	dll::dll(fmt::string_view path)
	{
		auto const buf = fmt::to_string(path);
		auto const s = data(buf);

		#ifdef _WIN32
		{
			auto const h = LoadLibrary(s);
			if (nullptr == h)
			{
				sys::win::err(here, "LoadLibrary", path);
			}
			else ptr = h;
		}
		#else
		{
			ptr = dlopen(s, RTLD_LAZY);
			if (nullptr == ptr)
			{
				auto const e = dlerror();
				::err(here, "dlopen", path, e);
			}
		}
		#endif
	}

	dll::~dll()
	{
		#ifdef _WIN32
		{
			auto const h = static_cast<HMODULE>(ptr);
			if (nullptr != h and not FreeLibrary(h))
			{
				sys::win::err(here, "FreeLibrary");
			}
		}
		#else
		{
			if (nullptr != ptr and dlclose(ptr))
			{
				auto const e = dlerror();
				::err(here, "dlclose", e);
			}
		}
		#endif
	}

	void *dll::sym(fmt::string_view name) const
	{
		auto const buf = fmt::to_string(name);
		auto const s = data(buf);

		#ifdef _WIN32
		{
			auto h = static_cast<HMODULE>(ptr);
			if (nullptr == h)
			{
				h = GetModuleHandle(nullptr);
				if (nullptr == h)
				{
					sys::win::err(here, "GetModuleHandle");
					return nullptr;
				}
			}

			auto const f = GetProcAddress(h, s);
			if (nullptr == f)
			{
				sys::win::err(here, "GetProcAddress", name);
			}
			return f;
		}
		#else
		{
			(void) dlerror();
			auto f = dlsym(ptr, s);
			auto const e = dlerror();
			if (nullptr != e)
			{
				::err(here, "dlsym", name, e);
			}
			return f;
		}
		#endif
	}

	dll dll::find(fmt::string_view basename)
	{
		using namespace ::env::dir;
		auto name = fmt::to_string(basename) + sys::ext::share;
		::env::dir::find(share, match(name) | copy(name) | stop);
		return fmt::string_view(name);
	}
}

