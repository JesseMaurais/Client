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
		using base = std::iterator_traits<Type>;
		using ref = base::references;
		using cref = base::const_reference;
		using ptr = base::pointer;
		using cptr = base::const_pointer;
		using pair = fwd::pair<Type>;
		using range = fwd::range<ptr>;
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
		using ios = memory<basic_ios>;
		using in = memory<basic_istream>;
		using out = memory<basic_ostream>;
		using io = memory<basic_iostream>;
		using buf = memory<basic_buf>;
		using file = memory<basic_file>;
	};

	template
	<
		class Type
	>
	struct struct_traits : memory_traits<Type>
	{
		using base = memory_traits<Type>;

		using base::node;

		template
		<
			template <class> Alloc = allocator
		>
		using edges = fwd::edges<Type, Alloc>;

		template
		<
			template <class> Alloc = allocator
		>
		using matrix = fwd::matrix<Type, Alloc>;

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
		class String, template <class> Alloc = allocator
	>
	struct string_type : String
	{ 
		using traits = compose
		<
			stream_traits<String::char_type, String::char_traits>,
			struct_traits<String>
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
		using traits::range;
		using traits::pair;
		using traits::list;
		using traits::span;
		using traits::node;
		using matrix = traits::matrix<Alloc>;
		using edges = traits::edges<Alloc>;
		using vector = traits::vector<Alloc>;

		template
		<
			class Sort = std::less
		>
		using set = traits::set<Sort, Alloc>;

		template 
		<
			class Sort = std::less
		> 
		using map = traits::map<Sort, Alloc>;

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
		template <class> Order = ordering,
		template <class> Alloc = allocator
	>
	struct basic_string_view : string_type<basic_string_view<Char, Traits>, Sort, Alloc>
	{
		using base = string_traits<fwd::basic_string_view<Char, Traits>, Sort, Alloc>;
		using base::base;
	};

	template
	<
		class Char,
		template <class> Traits = std::char_traits,
		template <class> Sort = std::less,
		template <class> Alloc = std::allocator
	>
	struct basic_string : string_traits<basic_string<Char, Traits, Alloc>, Sort, Alloc>
	{ 
		using base = string_traits<fwd::basic_string<Char, Traits, Alloc>, Sort, Alloc>;
		using base::base;
		using view = basic_string_view<Char, Traits, Sort, Alloc>;
	};

	using std::byte;
	using string = basic_string<char>;
	using wstring = basic_string<wchar_t>;
	using binary = basic_string<byte>;
	using ustring = basic_string<wint_t>;
}

#endif // file
