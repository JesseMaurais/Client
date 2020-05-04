#ifndef fmt_hpp
#define fmt_hpp "Standard Format"

#include "fwd.hpp"

namespace fmt
{
	template
	<
		class Type
	>
	struct memory_traits
	{
		using ptr = typename std::add_pointer<Type>::type;
		using cptr = typename std::add_pointer<typename std::add_const<Type>::type>::type;
		using ref = typename std::add_lvalue_reference<Type>::type;
		using cref = typename std::add_lvalue_reference<typename std::add_const<Type>::type>::type;
	};

	template
	<
		class Char, template <class> class Traits = fwd::character
	>
	struct stream_traits : memory_traits<Char>
	{
		using ios = memory_traits<fwd::basic_ios<Char, Traits>>;
		using in = memory_traits<fwd::basic_istream<Char, Traits>>;
		using out = memory_traits<fwd::basic_ostream<Char, Traits>>;
		using io = memory_traits<fwd::basic_iostream<Char, Traits>>;
		using buf = memory_traits<fwd::basic_buf<Char, Traits>>;
		using file = memory_traits<fwd::basic_file<Char, Traits>>;
		using str = memory_traits<fwd::basic_stringstream<Char, Traits>>;
	};

	template
	<
		class Type, 
		template <class> class Alloc = fwd::allocator,
		template <class, template <class> class> class Vector = fwd::vector,
		template <class> class Order = fwd::ordering
	>
	struct struct_traits : memory_traits<Type>
	{
		using pair = fwd::pair<Type>;
		using init = fwd::init<Type>;
		using span = fwd::span<Type>;
		using line = fwd::line<Type, Alloc, Vector>;
		using page = fwd::page<Type, Alloc, Vector>;
		using vector = fwd::vector<Type, Alloc>;
		using edges = fwd::edges<Type>;
		using graph = fwd::graph<Type, Alloc>;
		using group = fwd::group<Type, Alloc>;
		using set = fwd::set<Type, Order, Alloc>;
		using map = fwd::map<Type, Order, Alloc>;
	};

	template
	<
		class String, 
		class Char,
		template <class> class Traits = fwd::character,
		template <class> class Alloc = fwd::allocator,
		template <class, template <class> class> class Vector = fwd::vector,
		class Type = fwd::compose
		<
			struct_traits<String, Alloc, Vector>, stream_traits<Char, Traits>
		>
	>
	struct string_type : String, Type
	{ 
		using String::String;
		using stream = fwd::basic_stringstream<Char, Traits, Alloc>;
	};

	template
	<
		class Char,
		template <class> class Traits = fwd::character,
		template <class> class Alloc = fwd::allocator,
		template <class, template <class> class> class Vector = fwd::vector,
		class Type = string_type
		<
			fwd::basic_string_view<Char, Traits>, Char, Traits, Alloc, Vector
		>	
	>
	struct basic_string_view : Type
	{
		using Type::Type;
	};

	template
	<
		class Char,
		template <class> class Traits = fwd::character,
		template <class> class Alloc = fwd::allocator,
		template <class, template <class> class> class Vector = fwd::vector,
		class Type = string_type
		<
			fwd::basic_string<Char, Traits>, Char, Traits, Alloc, Vector
		>
	>
	struct basic_string : Type
	{
		using Type::Type;
		using view = basic_string_view<Char, Traits, Alloc, Vector>;
	};

	using string = 
	#ifdef _T
		basic_string<_T>;
	#else
		basic_string<char>;
	#endif

	using astring = basic_string<char>; // UTF-8 or low 7
	using wstring = basic_string<wchar_t>; // UTF-16 or UTF-32 wide
	using ustring = basic_string<wint_t>; // at least UTF-32 code point

	constexpr auto eol = '\n';
	constexpr auto tab = '\t';
	constexpr auto nil = '\0';
	constexpr auto npos = string::npos;
	constexpr auto null = size_t { };
}

#endif // file
