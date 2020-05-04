#ifndef fmt_hpp
#define fmt_hpp "Standard Format"

#include "fwd.hpp"
#include "algo.hpp"

namespace fmt
{
	template
	<
		class Type
	>
	struct memory_brief
	{
		using ptr  = typename std::add_pointer<Type>::type;
		using cptr = typename std::add_pointer<typename std::add_const<Type>::type>::type;
		using ref  = typename std::add_lvalue_reference<Type>::type;
		using cref = typename std::add_lvalue_reference<typename std::add_const<Type>::type>::type;
	};

	template
	<
		class Char, template <class> class Traits = fwd::character
	>
	struct stream_brief : memory_brief<Char>
	{
		using ios  = memory_brief<fwd::basic_ios<Char, Traits>>;
		using in   = memory_brief<fwd::basic_istream<Char, Traits>>;
		using out  = memory_brief<fwd::basic_ostream<Char, Traits>>;
		using io   = memory_brief<fwd::basic_iostream<Char, Traits>>;
		using buf  = memory_brief<fwd::basic_buf<Char, Traits>>;
		using file = memory_brief<fwd::basic_file<Char, Traits>>;
		using str  = memory_brief<fwd::basic_stringstream<Char, Traits>>;
	};

	template
	<
		class Type, 
		template <class> class Alloc = fwd::allocator,
		template <class, template <class> class> class Store = fwd::vector,
		template <class> class Order = fwd::ordering
	>
	struct struct_brief : memory_brief<Type>
	{
		using pair   = fwd::pair<Type>;
		using init   = fwd::init<Type>;
		using span   = fwd::span<Type>;
		using line   = fwd::line<Type, Alloc, Store>;
		using page   = fwd::page<Type, Alloc, Store>;
		using set    = fwd::set<Type, Order, Alloc>;
		using map    = fwd::map<Type, Order, Alloc>;
		using vector = fwd::vector<Type, Alloc>;
		using edges  = fwd::edges<Type>;
		using graph  = fwd::graph<Type, Alloc>;
		using group  = fwd::group<Type, Alloc>;
	};

	template
	<
		class String, 
		class Char,
		template <class> class Traits = fwd::character,
		template <class> class Alloc = fwd::allocator,
		template <class, template <class> class> class Vector = fwd::vector,
		// base class composition
		class Type = fwd::compose
		<
			struct_brief<String, Alloc, Vector>, stream_brief<Char, Traits>
		>
	>
	struct string_brief : String, Type
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
		// base class composition
		class Type = string_brief
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
		// base class composition
		class Type = string_brief
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

	static_assert(null == ~npos, "Need two's complement");
}

#endif // file
