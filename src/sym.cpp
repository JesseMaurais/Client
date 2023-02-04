// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "err.hpp"
#include "sym.hpp"
#include "sys.hpp"
#include "fmt.hpp"
#include "dir.hpp"
#include "env.hpp"
#include "type.hpp"

#ifdef _WIN32
#include "win.hpp"
#else
#include "uni.hpp"
#include <dlfcn.h>
#endif

namespace sys
{
	lib & bin()
	{
		static lib local;
		return local;
	}

	lib::lib(fmt::view path)
	{
		const auto buf = fmt::to_string(path);
		const auto s = buf.data();

		#ifdef _WIN32
		{
			const auto h = LoadLibrary(s);
			if (nullptr == h)
			{
				#ifdef WINERR
				WINERR("LoadLibrary", path);
				#endif
			}
			else ptr = h;
		}
		#else
		{
			ptr = dlopen(s, RTLD_LAZY);
			if (nullptr == ptr)
			{
				const auto e = dlerror();
				if (nullptr != e)
				{
					#ifdef trace
					trace(e);
					#endif
				}
			}
		}
		#endif
	}

	lib::~lib()
	{
		#ifdef _WIN32
		{
			const auto h = static_cast<HMODULE>(ptr);
			if (nullptr != h and not FreeLibrary(h))
			{
				#ifdef WINERR
				WINERR("FreeLibrary");
				#endif
			}
		}
		#else
		{
			if (nullptr != ptr and dlclose(ptr))
			{
				const auto e = dlerror();
				if (nullptr != e)
				{
					#ifdef trace
					trace(e);
					#endif
				}
			}
		}
		#endif
	}

	void *lib::sym(fmt::string::view name) const
	{
		const auto buf = fmt::to_string(name);
		const auto s = buf.data();

		#ifdef _WIN32
		{
			auto h = static_cast<HMODULE>(ptr);
			if (nullptr == h)
			{
				h = GetModuleHandle(nullptr);
				if (nullptr == h)
				{
					#ifdef WINERR
					WINERR("GetModuleHandle");
					#endif
					return nullptr;
				}
			}

			const auto f = GetProcAddress(h, s);
			if (nullptr == f)
			{
				#ifdef WINERR
				WINERR("GetProcAddress");
				#endif
			}
			return f;
		}
		#else
		{
			(void) dlerror();
			auto f = dlsym(ptr, s);
			const auto e = dlerror();
			if (nullptr != e)
			{
				#ifdef trace
				trace(e);
				#endif
			}
			return f;
		}
		#endif
	}

	lib lib::find(fmt::view basename)
	{
		using namespace env::file;
		fmt::string name = fmt::to_string(basename) + sys::tag::share;
		env::file::find(env::path(), regex(name) || to(name) || stop);
		return fmt::view(name);
	}
}

#ifdef TEST
static int hidden() { return 42; }
dynamic int visible() { return hidden(); }

TEST(sym)
{
	// Can see dynamic
	auto f = sys::sym<int()>("visible");
	assert(nullptr != f);
	// Cannot see static
	auto g = sys::sym<int()>("hidden");
	assert(nullptr == g);
	// Callable object
	assert(f() == hidden());
}
#endif
