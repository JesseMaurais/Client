#ifndef str_hpp
#define str_hpp

#include <string>
#if __has_include(<string_view>)
#include <string_view>
#elif __has_include(<experimental/string_view>)
#include <experimental/string_view>
namespace std
{
	using experimental::string_view;
	using experimental::wstring_view;
	template <typename... Args> using basic_string_view = experimental::basic_string_view<Args...>;
}
#else
#error You do not have a string view implementation.
#endif

#include <utility>
#include <vector>

namespace fmt
{
	using std::string;
	using std::wstring;
	using std::string_view;
	using std::wstring_view;
	using pair = std::pair<string, string>;
	using span_view = std::vector<string_view>;
}

#endif // file
