#ifndef str_hpp
#define str_hpp

#if __has_include(<experimental/string_view>)
#include <experimental/string_view>
namespace std
{
	using experimental::string_view;
	using experimental::wstring_view;
}
#elif __has_include(<string_view>)
#include <string_view>
#else
#error You do not have a string view implementation.
#endif

#endif // file
