#ifndef fmt_hpp
#define fmt_hpp "Standard Format"

#include "fwd.hpp"
#include "it.hpp"

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
	};

	template
	<
		class String, template <class> Alloc = allocator
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
		using traits::ref;
		using traits::cref;
		using traits::ptr;
		using traits::cptr;
		using traits::pair;
		using traits::init;
		using traits::span;
		using traits::edges;
		using graph = traits::graph<Alloc>;
		using group = traits::group<Alloc>;
		using vector = traits::vector<Alloc>;

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

		template
		<
			size_t Size
		> 
		using variant = traits::variant<Size, Alloc>;
	};

	template
	<
		class Char,
		template <class> Traits = character,
		template <class> Alloc = allocator
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
		template <class> Traits = character,
		template <class> Alloc = allocator
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

	using std::byte;

	using string = basic_string<char>;
	using wstring = basic_string<wchar_t>;
	using binary = basic_string<byte>;
	using ustring = basic_string<wint_t>;
}

#endif // file
