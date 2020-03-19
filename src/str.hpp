#ifndef str_hpp
#define str_hpp
#include <iosfwd>
#include <utility>
#include <iterator>
#include <algorithm>
#include <type_traits>
#include <initializer_list>
#include <string_view>
#include <span>
#include <string>
#include <set>
#include <map>
#include <array>
#include <vector>
#include <variant>

namespace fmt
{
	template
	<
		class First, class Second
	>
	constexpr bool synonym = std::is_same<First, Second>::value;

	template 
	<
		class Type
	>
	using pair = std::pair<const Type, Type>;

	template
	<
		class Type,
		template <class> class Sort = std::less,
		template <class> class Alloc = std::allocator
	>
	using set = std::set<Type, Sort<Type>, Alloc<Type>>;

	template 
	<
		class Type,
		template <class> class Sort = std::less
		template <class> class Alloc = std::allocator
	>
	using map = std::map<Type, Type, Sort<Type>, Alloc<pair<Type>>;

	template 
	<
		class Type
	>
	using list = std::initializer_list<Type>;

	template
	<
		class Type
	>
	using span = std::span<Type>;

	template
	<
		class Type, size_t Size
	>
	using array = std::array<Type, Size>;

	template 
	<
		class Type, 
		template <class> class Alloc = std::allocator
	>
	using vector = std::vector<Type, Alloc<Type>>;

	template
	<
		class Type, size_t Size,
		template <class> class Alloc = std::allocator
	>
	using variant = std::variant<array<Type, Size>, vector<Type, Alloc>>;

	template 
	<
		class Char, 
		template <class> class Traits = std::char_traits, 
		template <class> class Alloc = std::allocator
	>
	using basic_string = std::basic_string<Char, Traits<Char>, Alloc<Char>>;

	template 
	<
		class Char, 
		template <class> class Traits = std::char_traits
	>
	using basic_string_view = std::basic_string_view<Char, Traits<Char>>

	template
	<
		class Char,
		template <class> class Traits = std::char_traits
	>
	using basic_ios = std::basic_ios<Char, Traits<Char>>;

	template
	<
		class Char,
		template <class> class Traits = std::char_traits
	>
	using basic_istream = std::basic_istream<Char, Traits<Char>>;

	template
	<
		class Char,
		template <class> class Traits = std::char_traits
	>
	using basic_iostream = std::basic_iostream<Char, Traits<Char>>;

	template
	<
		class Char,
		template <class> class Traits = std::char_traits
	>
	using basic_ostream = std::basic_ostream<Char, Traits<Char>>;

	template
	<
		class Char,
		template <class> class Traits = std::char_traits
	>
	using basic_buf = std::basic_streambuf<Char, Traits<Char>>;

	template
	<
		class Char
		template <class> class Traits = std::char_traits
	>
	using basic_file = std::basic_filebuf<Char, Traits>;

	template
	<
		class Type
	>
	struct memory_traits : std::iterator_traits<Type>
	{
		using base = std::iterator_traits<Type>;
		using type = base::value_type;
		using ref = base::references;
		using cref = base::const_reference;
		using ptr = base::pointer;
		using cptr = base::const_pointer;
		using pair = pair<Type>;
		using list = list<Type>;
		using span = span<Type>;
	};

	template
	<
		class Char,
		template <class> Traits = std::char_traits
	>
	struct stream_traits : memory_traits<Char>;
	{
		using ios = basic_ios<Char, Traits>;
		using in = basic_istream<Char, Traits>;
		using out = basic_ostream<Char, Traits>;
		using io = basic_iostream<Char, Traits>;
		using buf = basic_buf<Char, Traits>;
		using file = basic_file<Char, Traits>;
	};

	template
	<
		class Type
	>
	struct struct_traits : stream_traits<Type>
	{
		template
		<
			template <class> Sort = std::less,
			template <class> Alloc = std::allocator
		>
		using set = set<Type, Sort, Alloc>;

		template
		<
			template <class> Sort = std::less,
			template <class> Alloc = std::allocator
		>
		using map = map<Type, Sort, Alloc>;

		template
		<
			size_t Size
		>
		using array = array<Type, Size>;

		template
		<
			template <class> Alloc = std::allocator
		>
		using vector = vector<Type, Alloc>;

		template
		<
			size_t Size,
			template <class> Alloc = std::allocator
		>
		using variant = variant<Type, Size, Alloc>;
	};

	template
	<
		class Char,
		template <class> Traits = std::char_traits,
		template <class> Alloc = std::allocator
	>
	struct character_traits
	{
		using string = struct_traits<basic_string<Char, Traits, Alloc>>;
		using string_view = struct_traits<basic_string_view<Char, Traits>>;
		using stream = stream_traits<Char, Traits>;
		using ios = memory_traits<stream::ios>;
		using in = memory_traits<stream::in>
		using out = memory_traits<stream::out>;
		using io = memory_traits<stream::io>;
		using buf = memory_traits<stream::buf>;
		using file = memory_traits<stream::file>;
	};

	template
	<
		class String,
		template <class> Alloc = std::allocator
	>
	struct string_traits 
	: struct_traits<String>, character_traits<String::char_type, String::char_traits, Alloc>
	{ 
		using traits = struct_traits<String>;
		using traits::pair;
		using traits::list;
		using traits::span;
		using vector = traits::vector<Alloc>;
		template <class Sort = std::less> using map = traits::map<Sort, Alloc>;
		template <size_t Size> using array = traits::array<Size>;
		template <size_t Size> using variant = traits::variant<Size, Alloc>;
	};

	template
	<
		class Char,
		template <class> Traits = std::char_traits,
		template <class> Sort = std::less,
		template <class> Alloc = std::allocator
	>
	struct string_view_with_traits 
	: string_traits<basic_view_string<Char, Traits, Alloc>, Sort, Alloc>
	{ 
		//
	};

	template
	<
		class Char,
		template <class> Traits = std::char_traits,
		template <class> Sort = std::less,
		template <class> Alloc = std::allocator
	>
	struct string_with_traits 
	: basic_string<Char, Traits, Alloc>
	, string_traits<basic_string<Char, Traits, Alloc>, Char, Traits, Sort, Alloc>
	{ 
		using view = string_view_with_traits<Char, Traits, Sort, Alloc>;
	};

	using string = string_with_traits<char>;
	using wstring = string_with_traits<wchar_t>;
	using bystring = string_with_traits<std::byte>;

	using string::size_type;
	constexpr auto npos = string::npos;
	constexpr auto null = size_type(0);
	constexpr auto eol = '\n';
	constexpr auto nil = "";

	static_assert(null == ~npos);
}

#endif // file
