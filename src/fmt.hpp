#ifndef fmt_hpp
#define fmt_hpp

#include "fwd.hpp"
#include <string_view>
#include <string>

namespace fmt
{
	template
	<
		class Type
	>
	struct memory_traits : std::iterator_traits<Type>
	{
		using base = std::iterator_traits<Type>;
		using ref = base::references;
		using cref = base::const_reference;
		using ptr = base::pointer;
		using cptr = base::const_pointer;
		using pair = fwd::pair<value>;
		using range = fwd::range<cptr>;
		using init = fwd::init<value>;
		using span = fwd::span<value>;
	};

	template
	<
		class Char,
		template <class> Traits = std::char_traits
	>
	struct stream_traits : memory_traits<Char>;
	{
		template 
		<
			template <class, template <class> class> class Stream
		>
		using traits = memory_traits<Stream<Char, Traits>>;
		using ios = traits<basic_ios>;
		using in = traits<basic_istream>;
		using out = traits<basic_ostream>;
		using io = traits<basic_iostream>;
		using buf = traits<basic_buf>;
		using file = traits<basic_file>;
	};

	template
	<
		class Type
	>
	struct struct_traits : memory_traits<Type>
	{
		using base = memory_traits<Type>;

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
		class String,
		template <class> Alloc = std::allocator
	>
	struct string_traits : String
	{ 
		using stream = stream_traits<String::char_type, String::char_traits>;
		using stream::ios;
		using stream::in;
		using stream::out;
		using stream::io;
		using stream::buf;
		using stream::file;

		using traits = struct_traits<String>;
		using traits::ref;
		using traits::cref;
		using traits::ptr;
		using traits::cptr;
		using traits::range;
		using traits::pair;
		using traits::list;
		using traits::span;
		using traits::node;
		using graph = traits::graph<Alloc>;
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
		template <class> Traits = std::char_traits,
		template <class> Sort = std::less,
		template <class> Alloc = std::allocator
	>
	struct basic_string_view : string_traits<basic_string_view<Char, Traits>, Sort, Alloc>
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
//	using binary = basic_string<byte>;
//	using ustring = basic_string<wint_t>;
}

#endif // file
