#ifndef fmt_hpp
#define fmt_hpp

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
		class Type,
		template <class> class Sort = std::less,
		template <class> class Alloc = std::allocator
	>
	using node = std::pair<Type, set<Type, Sort, Alloc>>;

	template
	<
		class Type,
		template <class> class Sort = std::less,
		template <class> class Alloc = std::allocator
	>
	using graph = set<pair<Type>, Sort, Alloc>;

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
		class Iterator
	>
	struct range : pair<Iterator>
	{
		using base = pair<Iterator>;

		auto begin() const
		{
			return base::first;
		}

		auto end() const
		{
			return base::second;
		}
	};

	template
	<
		class Type
	>
	struct memory_traits : std::iterator_traits<Type>
	{
		using base = std::iterator_traits<Type>;
		using value = base::value_type;
		using ref = base::references;
		using cref = base::const_reference;
		using ptr = base::pointer;
		using cptr = base::const_pointer;
		using range = range<ptr>;
		using pair = pair<value>;
		using list = list<value>;
		using span = span<value>;
		using node = node<value>;
		using size = std::size_t;
		using diff = base::difference_type;
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
			template <class, template <class> class> class Stream,
			class Char, template <class> class Traits
		>
		using traits = memory_traits<Stream<Char, Traits>>;

		using base = memory_traits<Char>;
		using ios = traits<basic_ios, Char, Traits>;
		using in = traits<basic_istream, Char, Traits>;
		using out = traits<basic_ostream, Char, Traits>;
		using io = traits<basic_iostream, Char, Traits>;
		using buf = traits<basic_buf, Char, Traits>;
		using file = traits<basic_file, Char, Traits>;
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
		using base = string_traits<basic_string_view<Char, Traits>, Sort, Alloc>;
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
		using base = string_traits<basic_string<Char, Traits, Alloc>, Sort, Alloc>;
		using base::base;
		using view = basic_string_view<Char, Traits, Sort, Alloc>;
	};

	using std::byte;
	using string = basic_string<char>;
	using wstring = basic_string<wchar_t>;
//	using binary = basic_string<byte>;
//	using ustring = basic_string<wint_t>;

	using string::size_type;
	constexpr auto npos = string::npos;
	constexpr auto null = size_type(0);
	constexpr auto eol = '\n';
	constexpr auto nil = "";

	static_assert(null == ~npos);
}

#endif // file
