#ifndef str_hpp
#define str_hpp

// Try to get the lightest of string containers, a span of string views.
// Pull in string buffers and pairs as well for type completeness.

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
#error Cannot find an implementation of std::string_view.
#endif

#if __has_include(<span>)
#include <span>
namespace fmt
{
	using span_view = std::span<fmt::string_view>;
	using wspan_view = std::span<fmt::wstring_view>;
	template <typename... Args> using basic_span_view = std::span<basic_string_view<Args...>>;
}
#elif __has_include(<experimetal/span>)
#include <experimental/span>
namespace fmt
{
	using span_view = std::experimental::span<fmt::string_view>;
	using wspan_view = std::experimental::span<fmt::wstring_view>;
	template <typename... Args> using basic_span_view = std::span<basic_string_view<Args...>>;
}
#else
//#warning Cannot find an implementation of std::span. Using std::vector instead.
#include <vector>
namespace fmt
{
	using span_view = std::vector<fmt::string_view>;
	using wspan_view = std::vector<fmt::wstring_view>;
	template <typename... Args> using basic_span_view = std::vector<basic_string_view<Args...>>;
}
#endif

#include <utility>
#include <string>

namespace fmt
{
	using std::string;
	using std::wstring;

	using pair = std::pair<string, string>;
	using wpair = std::pair<wstring, wstring>;
	using pair_view = std::pair<string_view, string_view>;
	using wpair_view = std::pair<wstring_view, wstring_view>;
}

#endif // file
