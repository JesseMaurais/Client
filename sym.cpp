#include "sym.hpp"
#include "os.hpp"
#include "fmt.hpp"
#include "dbg.hpp"
#include <iostream>

#if __has_include(<dlfcn.h>)
#include <dlfcn.h>
#else
#include "dlfcn.c"
#endif

using namespace fmt;

namespace
{
	inline void error(string_view who, string_view what)
	{
		if constexpr (DEBUG)
		{
			std::cerr << who << ": " << what << std::endl;
		}
	}
}

namespace sys
{
	sym::operator bool() const
	{
		return nullptr != dl;
	}

	sym::sym()
	{ 
		dl = ::dlopen(nullptr, RTLD_LAZY);
		if (nullptr == dl)
		{
			string const e = ::dlerror();
			error("dlopen", e);
		}
	}

	sym::sym(string_view path)
	{
		auto const buf = fmt::to_string(path);
		auto const s = buf.c_str();
		dl = ::dlopen(s, RTLD_LAZY);
		if (nullptr == dl)
		{
			string const e = ::dlerror();
			error("dlopen", e);
		}
	}

	sym::~sym()
	{
		if (dl and ::dlclose(dl))
		{
			string const e = ::dlerror();
			error("dlclose", e);
		}
	}

	void *sym::link(string_view name)
	{
		auto const buf = fmt::to_string(name);
		auto const s = buf.c_str();
		// see pubs.opengroup.org
		(void) ::dlerror();
		auto f = ::dlsym(dl, s);
		string const e = ::dlerror();
		if (not empty(e))
		{
			error("dlsym", e);
		}
		return f;
	}
}

