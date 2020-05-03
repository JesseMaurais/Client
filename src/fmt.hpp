#ifndef fmt_hpp
#define fmt_hpp "Standard Format"

#include "fwd.hpp"

namespace fmt
{
	template
	<
		class Type, template <class> class Traits = fwd::iterator
	>
	struct memory_traits : Traits<Type>
	{
		using ref = typename Traits<Type>::reference;
		using cref = typename Traits<Type>::const_reference;
		using ptr = typename Traits<Type>::pointer;
		using cptr = typename Traits<Type>::const_pointer;
	};

	template
	<
		class Char, 
		template <class> class Traits = fwd::character,
		template <class> class Iterator = fwd::iterator
	>
	struct stream_traits : memory_traits<Char, Iterator>
	{
		using ios = memory_traits<fwd::basic_ios<Char, Traits>, Iterator>;
		using in = memory_traits<fwd::basic_istream<Char, Traits>, Iterator>;
		using out = memory_traits<fwd::basic_ostream<Char, Traits>, Iterator>;
		using io = memory_traits<fwd::basic_iostream<Char, Traits>, Iterator>;
		using buf = memory_traits<fwd::basic_buf<Char, Traits>, Iterator>;
		using file = memory_traits<fwd::basic_file<Char, Traits>, Iterator>;
		using str = memory_traits<fwd::basic_stringstream<Char, Traits>, Iterator>;
	};

	template
	<
		class Type
	>
	struct struct_traits : memory_traits<Type>
	{
		using pair = fwd::pair<Type>;
		using init = fwd::init<Type>;
		using span = fwd::span<Type>;
		using edges = fwd::edges<Type>;

		template
		<
			template <class> Alloc = allocator
		>
		using graph = fwd::graph<Type, Alloc>;

		template
		<
			template <class> Order = ordering,
			template <class> Alloc = allocator
		>
		using group = fwd::group<Type, Order, Alloc>;

		template
		<
			template <class> Order = ordering,
			template <class> Alloc = allocator
		>
		using set = fwd::set<Type, Order, Alloc>;

		template
		<
			template <class> Order = ordering,
			template <class> Alloc = allocator
		>
		using map = fwd::map<Type, Order, Alloc>;

		template
		<
			size_t Size
		>
		using array = fwd::array<Type, Size>;

		template
		<
			template <class> Alloc = allocator
		>
		using vector = fwd::vector<Type, Alloc>;

		template
		<
			size_t Size,
			template <class> Alloc = allocator
		>
		using variant = fwd::variant<Type, Size, Alloc>;

		template
		<
			template <class> class Alloc = allocator,
			template <class, class> class Vector = vector
		>
		using line = fwd::line<Type, Alloc, Vector>;

		template
		<
			template <class> class Alloc = allocator,
			template <class, class> class Vector = vector
		>
		using page = fwd::page<Type, Alloc, Vector>;
	};

	template
	<
		class String, 
		template <class> Alloc = allocator,
		template <class, class> Vector = vector
	>
	struct string_type : String
	{ 
		using traits = compose
		<
			struct_traits<String>, stream_traits
			<
				String::char_type, String::char_traits
			>
		>;

		using traits::char_type;
		using traits::char_traits;
		using traits::ios;
		using traits::in;
		using traits::out;
		using traits::io;
		using traits::buf;
		using traits::file;
		using traits::str;
		using traits::ref;
		using traits::cref;
		using traits::ptr;
		using traits::cptr;
		using traits::init;
		using traits::span;
		using traits::edges;
		using graph = traits::graph<Alloc>;
		using group = traits::group<Alloc>;
		using vector = traits::vector<Alloc>;
		using line = traits::line<Alloc, Vector>;
		using page = traits::page<Alloc, Vector>;
		using pair = fwd::pair<String::size_type>;
		using stream = fwd::basic_stringstream<char_type, char_traits, Alloc>;

		template
		<
			template <class> class Order = ordering
		>
		using set = traits::set<Order, Alloc>;

		template 
		<
			template <class> class Order = ordering
		> 
		using map = traits::map<Order, Alloc>;

		template 
		<
			size_t Size
		> 
		using array = traits::array<Size>;
	};

	template
	<
		class Char,
		template <class> Traits = fwd::character,
		template <class> Alloc = fwd::allocator
	>
	struct basic_string_view : string_type
	<
		fwd::basic_string_view<Char, Traits>, Alloc
	>
	{
		using base = string_traits
		<
			fwd::basic_string_view<Char, Traits>, Alloc
		>;
		using base::base;
	};

	template
	<
		class Char,
		template <class> Traits = fwd::character,
		template <class> Alloc = fwd::allocator
	>
	struct basic_string : string_type
	<
		fwd::basic_string<Char, Traits, Alloc>, Alloc
	>
	{
		using view = basic_string_view<Char, Traits, Alloc>;
		using base = string_traits
		<
			fwd::basic_string<Char, Traits, Alloc>, Alloc
		>;
		using base::base;
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
}

#endif // file
