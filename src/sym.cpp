// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "sym.hpp"
#include "env.hpp"
#include "sys.hpp"
#include "fmt.hpp"
#include "dir.hpp"
#include "err.hpp"
#include "type.hpp"
#ifdef _WIN32
#include "win.hpp"
#else
#include "uni.hpp"
#include <dlfcn.h>
#endif

namespace
{
	sys::dll const current;
}

namespace sys
{
	dll::cref self = current;

	dll::dll(fmt::string::view path)
	{
		auto const buf = fmt::to_string(path);
		auto const s = buf.data();

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
				sys::warn(here, "dlopen", path, e);
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
				sys::warn(here, "dlclose", e);
			}
		}
		#endif
	}

	void *dll::sym(fmt::string::view name) const
	{
		auto const buf = fmt::to_string(name);
		auto const s = buf.data();

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
				sys::warn(here, "dlsym", name, e);
			}
			return f;
		}
		#endif
	}

	dll dll::find(fmt::string::view basename)
	{
		using namespace env::dir;
		fmt::string name = fmt::to_string(basename) + sys::ext::share;
		env::dir::find(env::paths(), regx(name) || to(name) || stop);
		return fmt::string::view(name);
	}
}

#ifdef test
static int hidden() { return 42; }
dynamic int visible() { return hidden(); }

test(sym)
{
	// Can see dynamic)
	auto f = sys::sym<int()>("visible");
	assert(nullptr != f);
	// Cannot see static
	auto g = sys::sym<int()>("hidden");
	assert(nullptr != g);
	// Callable object
	assert(f() == hidden());
	// Not callable
	except(g() == hidden());
}
#endif
