// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dll.hpp"
#include "fmt.hpp"
#include "dir.hpp"
#include <iostream>

#ifdef _WIN32
# include "dlfcn.c"
#else
# include <dlfcn.h>
#endif

using namespace fmt;

namespace
{
	inline void error(char const* who, char const* what)
	{
		std::cerr << who << ": " << what << std::endl;
	}

	fmt::string expr(fmt::string_view name)
	{
		#ifdef _WIN32
		{
			return fmt::to_string(name) + ".dll";
		}
		#else
		{
			return fmt::to_string(name) + ".so";
		}
		#endif
	}
}

namespace sys
{
	dll::operator bool() const
	{
		return RTLD_DEFAULT != tab;
	}

	dll::dll() : tab(RTLD_DEFAULT)
	{ }

	dll::dll(fmt::string_view path)
	{
		auto const buf = fmt::to_string(path);
		auto const s = buf.empty() ? nullptr : buf.c_str();
		tab = dlopen(s, RTLD_LAZY);
		if (nullptr == tab)
		{
			error("dlopen", dlerror());
		}
	}

	dll::~dll()
	{
		if (tab and dlclose(tab))
		{
			error("dlclose", dlerror());
		}
	}

	void *dll::sym(string_view name) const
	{
		auto const buf = fmt::to_string(name);
		auto const s = buf.c_str();
		// see pubs.opengroup.org
		(void) dlerror();
		auto f = dlsym(tab, s);
		auto const e = dlerror();
		if (nullptr != e)
		{
			error("dlsym", e);
		}
		return f;
	}

	dll dll::find(string_view name)
	{
		fmt::string path;
		::env::dir::find(::env::dir::share, ::env::dir::regex(expr(name)) | ::env::dir::name(path));
		return fmt::string_view(path);
	}
}

