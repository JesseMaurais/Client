// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dll.hpp"
#include "sys.hpp"
#include "fmt.hpp"
#include "dir.hpp"
#include "err.hpp"
#include <iostream>

#ifdef _WIN32
# include "win.hpp"
#else
# include <dlfcn.h>
#endif

namespace
{
	#ifndef _WIN32
	template <typename... Args> inline void error(Args... args)
	{
		if (sys::debug)
		{
			std::cerr << fmt::error(args...) << std::endl;
		}
	}
	#endif
}

namespace sys
{
	dll::dll(fmt::string_view path)
	{
		auto const buf = fmt::to_string(path);
		auto const s = buf.c_str();

		#ifdef _WIN32
		{
			auto const h = LoadLibrary(s);
			if (nullptr == h)
			{
				sys::win::perror(here, "LoadLibrary", path);
			}
			else ptr = h;
		}
		#else
		{
			ptr = dlopen(s, RTLD_LAZY);
			if (nullptr == ptr)
			{
				error(here, "dlopen", path, dlerror());
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
				sys::win::perror(here, "FreeLibrary");
			}
		}
		#else
		{
			if (nullptr != ptr and dlclose(ptr))
			{
				error(here, "dlclose", dlerror());
			}
		}
		#endif
	}

	void *dll::sym(fmt::string_view name) const
	{
		auto const buf = fmt::to_string(name);
		auto const s = buf.c_str();

		#ifdef _WIN32
		{
			auto h = static_cast<HMODULE>(ptr);
			if (nullptr == h)
			{
				h = GetModuleHandle(nullptr);
				if (nullptr == h)
				{
					sys::win::perror(here, "GetModuleHandle");
					return nullptr;
				}
			}

			auto const f = GetProcAddress(h, s);
			if (nullptr == f)
			{
				sys::win::perror(here, "GetProcAddress", name);
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
				error(here, "dlsym", name, e);
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

