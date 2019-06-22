#include "sym.hpp"
#include "os.hpp"
#include "dbg.hpp"
#include <iostream>

#ifdef __POSIX__
#include <dlfcn.h>
#else
#ifdef __WIN32__
namespace
{
#include "dlfcn.c"
}
#endif
#endif

namespace
{
	inline void err(fmt::string_view what, fmt::string_view who)
	{
		if constexpr (DEBUG)
		{
			std::cerr << what << ": " << who << ": " << ::dlerror() << std::endl;
		}
	}
}

namespace sys
{
	sym::operator bool() const
	{
		return nullptr != dl;
	}

	sym::sym(fmt::string_view path)
	: image(to_string(path))
	{
		auto const s = image.c_str();
		dl = ::dlopen(s);
		if (not dl)
		{
			::err("dlopen", image);
		}
	}

	sym::~sym()
	{
		if (dl and ::dlclose(dl))
		{
			::err("dlclose", image);
		}
	}

	void *sym::find(fmt::string_view name)
	{
		auto const index = to_string(name);
		auto const s = index.c_str();
		auto fn = ::dlsym(dl, s);
		if (not fn)
		{
			::err("dlsym", index);
		}
		// see pubs.opengroup.org
		return *(void**)(&fn);
	}
}

