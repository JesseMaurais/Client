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

	template <class Char> using basic_pair = std::pair<std::basic_string<Char>, std::basic_string<Char>>;
	template <class Char> using basic_pair_view = std::pair<basic_string_view<Char>, basic_string_view<Char>>;

	using pair = basic_pair<char>;
	using wpair = basic_pair<wchar_t>;
	using pair_view = basic_pair_view<char>;
	using wpair_view = basic_pair_view<wchar_t>;
}

#endif // file
