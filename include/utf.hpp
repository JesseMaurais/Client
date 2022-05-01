#ifndef utf_hpp
#define utf_hpp "Universal Transformation Format"

#include "ptr.hpp"
#include <uchar.h>
#include <wchar.h>
#include <cstring>

namespace fmt
{
	struct utf : fwd::zero<std::mbstate_t>
	{
		operator bool() const
		{
			return std::mbsinit(this);
		}

		std::ptrdiff_t len(char const* s, std::size_t n = MB_CUR_MAX)
		{
			return std::mbrlen(s, n, this);
		}

		std::ptrdiff_t to(wchar_t* w, char const* s, std::size_t n = MB_CUR_MAX)
		{
			return std::mbrtowc(w, s, n, this);
		}

		std::ptrdiff_t to(char* s, wchar_t w)
		{
			return std::wcrtomb(s, w, this);
		}

		std::ptrdiff_t to(wchar_t* w, char const** s, std::size_t n)
		{
			return std::mbsrtowcs(w, s, n, this);
		}

		std::ptrdiff_t to(char* s, wchar_t const** w, std::size_t n)
		{
			return std::wcsrtombs(s, w, n, this);
		}
	};
}

#endif // file
