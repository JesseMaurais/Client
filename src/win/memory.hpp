#ifndef win_memory_hpp
#define win_memory_hpp "WIN32 Shared Memory"

#include "win.hpp"
#include "ptr.hpp"

namespace sys::win::mem
{
	template <class Type> auto make_ptr(Type* ptr)
	{
		return fwd::make_ptr(ptr, [](auto ptr)
		{
			if (nullptr != ptr)
			{
				if (not UnmapViewOfFile(ptr))
				{
					sys::win::err(here, "UnmapViewOfFile", ptr);
				}
			}
		});
	}

	template <class Type = void> auto map(HANDLE h, DWORD dw, off_t off = 0, size_t sz = 0, Type* ptr = nullptr)
	{
		ptr = MapViewOfFileEx(h, dw, HIWORD(off), LOWORD(off), sz, ptr);
		if (nullptr == ptr)
		{
			sys::win::err(here, "MapViewOfFileEx", dw, off, sz, ptr);
		}
		return make_ptr(ptr);
	}
}

#endif
