#ifndef str_hpp
#define str_hpp

// Try to get the lightest of string containers: a span of string views.

#include <string>
#include <vector>
#include <utility>

#if __has_include(<string_view>)
#include <string_view>
namespace fmt::impl
{
	template <class Char, class Traits = std::char_traits<Char>> 
	using basic_string_view = std::basic_string_view<Char, Traits>;
}
#elif __has_include(<experimental/string_view>)
#include <experimental/string_view>
namespace fmt::impl
{
	template <class Char, class Traits = std::char_traits<Char>> 
	using basic_string_view = std::experimental::basic_string_view<Char, Traits>;
}
#else
#error Cannot find an implementation of string_view.
#endif

namespace fmt
{
	template <class Char, class Traits = std::char_traits<Char>>
	struct basic_string_view : impl::basic_string_view<Char, Traits>
	{
		using string = std::basic_string<Char, Traits>;
		using base = impl::basic_string_view<Char, Traits>;
		using base::base;

		#if __cplusplus <= 201703L
		bool starts_with(basic_string_view u) const
		{
			return 0 == base::compare(0, u.size(), u);
		}
		bool ends_with(basic_string_view u) const
		{
			auto const n = u.size();
			auto const m = base::size();
			return n <= m and 0 == base::compare(m - n, n, u);
		}
		#endif

		basic_string_view(string const& s)
		: base(s.data(), s.size())
		{ }

		basic_string_view(base u)
		: base(u.data(), u.size())
		{ }
	};

	template <class Char, class Traits = std::char_traits<Char>, class Alloc = std::allocator<Char>>
	struct basic_string : std::basic_string<Char, Traits, Alloc>
	{
		using view = impl::basic_string_view<Char, Traits>;
		using base = std::basic_string<Char, Traits, Alloc>;
		using base::base;

		basic_string(base const& s)
		: base(s)
		{ }

		basic_string(view u)
		: base(u.data(), u.size())
		{ }

		auto& operator=(base const& s)
		{
			return base::operator=(s);
		}

		operator view() const
		{
			return view(base::data(), base::size());
		}
	};

	template <class Type> using pair = std::pair<Type, Type>;

	template <class Iterator> struct range : pair<Iterator>
	{
		using base = pair<Iterator>;
		using base::base;
		using iterator = Iterator;

		auto begin() const
		{
			return base::first;
		}

		auto end() const
		{
			return base::second;
		}

		bool empty() const
		{
			return base::first == base::second;
		}

		std::size_t size() const
		{
			return std::distance(base::first, base::second);
		}
	};
}

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
#include <initializer_list>
namespace fmt
{
	template <typename Type> struct span : range<Type const*>
	{
		using base = range<Type const*>;
		using base::base;
		using const_iterator = Type const*;

		span(std::vector<Type> const& list)
		: base(data(list), data(list) + size(list))
		{ }

		span(std::initializer_list<Type> init)
		: base(begin(init), end(init))
		{ }

		const auto& front() const
		{
			return *base::first;
		}

		const auto& back() const
		{
			return *(base::second - 1);
		}

		auto const& operator[](std::size_t index) const
		{
			return *(base::first + index);
		}
	};
}
#endif

namespace fmt
{
	template <class Char> using basic_string_pair = pair<basic_string<Char>>;
	template <class Char> using basic_string_vector = typename std::vector<basic_string<Char>>;
	template <class Char> using basic_string_span = span<basic_string<Char>>;

	template <class Char> using basic_string_view_pair = pair<basic_string_view<Char>>;
	template <class Char> using basic_string_view_vector = typename std::vector<basic_string_view<Char>>;
	template <class Char> using basic_string_view_span = span<basic_string_view<Char>>;

	using string = basic_string<char>;
	using wstring = basic_string<wchar_t>;

	using string_view = basic_string_view<char>;
	using wstring_view = basic_string_view<wchar_t>;

	using string_pair = basic_string_pair<char>;
	using wstring_pair = basic_string_pair<wchar_t>;

	using string_vector = basic_string_vector<char>;
	using wstring_vector = basic_string_vector<wchar_t>;

	using string_span = basic_string_span<char>;
	using wstring_span = basic_string_span<wchar_t>; 

	using string_view_pair = basic_string_view_pair<char>;
	using wstring_view_pair = basic_string_view_pair<wchar_t>;

	using string_view_vector = basic_string_view_vector<char>;
	using wstring_view_vector = basic_string_view_vector<wchar_t>;

	using string_view_span = basic_string_view_span<char>;
	using wstring_view_span = basic_string_view_span<wchar_t>;

	using size_type = string::size_type;
	using size_pair = pair<size_type>;

	constexpr auto npos = string::npos;
	constexpr decltype(npos) null { };
	constexpr auto eol = '\n';
	constexpr auto nil = "";
}

#endif // file
