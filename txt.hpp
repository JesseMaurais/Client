#ifndef txt_hpp
#define txt_hpp

#include "str.hpp"

namespace txt
{
	using fmt::string_view;
	using fmt::wstring_view;

	string_view get(string_view);
	wstring_view get(wstring_view);
}

#endif // file
