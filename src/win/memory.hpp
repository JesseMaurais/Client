#ifndef win_mem_hpp
#define win_mem_hpp

#include "win.hpp"
#include "ptr.hpp"

namespace sys::win
{
	auto make_map(HANDLE h, DWORD dw, off_t off = 0, size_t sz = 0, void *ptr = nullptr)
	{
		ptr = MapViewOfFileEx(h, dw, HIWORD(off), LOWORD(off), sz, ptr);
		if (nullptr == ptr)
		{
			sys::win::err(here, "MapViewOfFileEx", dw, off, sz, ptr);
		}

		return make_ptr(ptr, [](auto ptr)
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
}

#endif
