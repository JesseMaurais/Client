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
		using base = Traits<Type>;
		using ref = base::references;
		using cref = base::const_reference;
		using ptr = base::pointer;
		using cptr = base::const_pointer;
		using pair = fwd::pair<Type>;
		using init = fwd::init<Type>;
		using span = fwd::span<Type>;
	};

	template
	<
		class Char, 
		template <class> Traits = fwd::character
		template <class> Iterator = fwd::iterator
	>
	struct stream_traits : memory_traits<Char, Iterator>;
	{
		template 
		<
			template <class, template <class> class> class Stream
		>
		using memory = memory_traits<Stream<Char, Traits>, Iterator>;
		using ios = memory<fwd::basic_ios>;
		using in = memory<fwd::basic_istream>;
		using out = memory<fwd::basic_ostream>;
		using io = memory<fwd::basic_iostream>;
		using buf = memory<fwd::basic_buf>;
		using file = memory<fwd::basic_file>;
		using str = memory<fwd::basic_stringstream>;
	};

	template
	<
		class Type
	>
	struct struct_traits : memory_traits<Type>
	{
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
}

#endif // file
