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
	template <typename Type> using span = std::span<Type>;
}
#elif __has_include(<experimetal/span>)
#include <experimental/span>
namespace fmt
{
	template <typename Type> using span = std::experimental::span<Type>;
}
#else
//#warning Cannot find an implementation of std::span. Using std::vector instead.
#include <vector>
namespace fmt
{
	template <typename Type> using span = std::vector<Type>;
}
#endif

#include <utility>
#include <string>
#include <vector>

namespace fmt
{
	template <class Type> using pair = std::pair<Type, Type>;
	template <class Type> using range = pair<typename Type::iterator>;

	template <class Char> using basic_string = std::basic_string<Char>;
	template <class Char> using basic_string_pair = pair<basic_string<Char>>;
	template <class Char> using basic_string_view_pair = pair<basic_string_view<Char>>;
	template <class Char> using basic_string_size_pair = pair<typename basic_string_view<Char>::size_type>;
	template <class Char> using basic_string_vector = typename std::vector<basic_string<Char>>;
	template <class Char> using basic_string_view_vector = typename std::vector<basic_string_view<Char>>;
	template <class Char> using basic_string_span = span<basic_string<Char>>;
	template <class Char> using basic_string_view_span = span<basic_string_view<Char>>;
	template <class Char> using basic_string_range = range<basic_string<Char>>;
	template <class Char> using basic_string_vector_range = range<basic_string_vector<Char>>;
	template <class Char> using basic_string_view_range = range<basic_string_view<Char>>;
	template <class Char> using basic_string_view_span_range = range<basic_string_view_span<Char>>;

	template <class Char>
	inline bool same(basic_string_view<Char> u, basic_string_view<Char> v)
	{
		return u.empty() ? v.empty() : u.size() == v.size() and u.data() == v.data();
	}

	using std::string;
	using std::wstring;

	using string_pair = basic_string_pair<char>;
	using wstring_pair = basic_string_pair<wchar_t>;

	using string_view_pair = basic_string_view_pair<char>;
	using wstring_view_pair = basic_string_view_pair<wchar_t>;

	using string_size_pair = basic_string_size_pair<char>;
	using wstring_size_pair = basic_string_size_pair<wchar_t>;

	using string_vector = basic_string_vector<char>;
	using wstring_vector = basic_string_vector<wchar_t>;

	using string_view_vector = basic_string_view_vector<char>;
	using wstring_view_vector = basic_string_view_vector<wchar_t>;

	using string_span = basic_string_span<char>;
	using wstring_span = basic_string_span<wchar_t>; 

	using string_view_span = basic_string_view_span<char>;
	using wstring_view_span = basic_string_view_span<wchar_t>;

	using string_range = basic_string_vector_range<char>;
	using wstring_range = basic_string_vector_range<wchar_t>;

	using string_view_range = basic_string_view_range<char>;
	using wstring_view_range = basic_string_view_range<wchar_t>;

	using string_view_span_range = basic_string_view_span_range<char>;
	using wstring_view_span_range = basic_string_view_span_range<wchar_t>;

	using size = string::size_type;
	constexpr auto npos = string::npos;
}

#endif // file
