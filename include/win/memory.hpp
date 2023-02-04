#ifndef win_memory_hpp
#define win_memory_hpp "WIN32 Shared Memory"

#include "win.hpp"
#include "ptr.hpp"

namespace sys::win::mem
{
	template <class Type> auto make_unique(fwd::as_ptr<Type> ptr)
	{
		return fwd::make_unique<Type>(ptr, [](auto ptr)
		{
			if (nullptr != ptr)
			{
				if (not UnmapViewOfFile(ptr))
				{
					#ifdef WINERR
					WINERR("UnmapViewOfFile", ptr);
					#endif
				}
			}
		});
	}

	template <class Type = void> auto map(HANDLE h, DWORD dw, off_t off = 0, size_t sz = 0, fwd::as_ptr<Type> ptr = nullptr)
	{
		auto lp = MapViewOfFileEx(h, dw, HIWORD(off), LOWORD(off), SIZE_T(sz), LPVOID(ptr));
		ptr = fwd::cast_as<Type>(lp);
		if (nullptr == ptr)
		{
			#ifdef WINERR
			WINERR("MapViewOfFileEx", dw, off, sz, ptr);
			#endif
		}
		return make_unique<Type>(ptr);
	}
}

#endif
