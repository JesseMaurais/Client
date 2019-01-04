#ifndef str_hpp
#define str_hpp

#if __has_include(<string_view>)
#include <string_view>
namespace fmt
{
	using std::string_view;
	using std::wstring_view;
	template <typename... Args> using basic_string_view = std::basic_string_view<Args...>;
}
#elif __has_include(<experimental/string_view>)
#include <experimental/string_view>
namespace fmt
{
	using std::experimental::string_view;
	using std::experimental::wstring_view;
	template <typename... Args> using basic_string_view = std::experimental::basic_string_view<Args...>;
}
#else
#error You do not have a string view implementation.
#endif

#include <utility>
#include <vector>
#include <string>

namespace fmt
{
	using std::string;
	using std::wstring;
	using pair = std::pair<string, string>;
	using span_view = std::vector<string_view>;
}

#endif // file
