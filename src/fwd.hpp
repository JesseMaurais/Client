#ifndef fwd_hpp
#define fwd_hpp

#include <iosfwd>
#include <utility>
#include <iterator>
#include <algorithm>
#include <type_traits>
#include <initializer_list>
#include <set>
#include <map>
#include <span>
#include <array>
#include <vector>
#include <variant>

namespace fwd
{
	template 
	<
		class Type
	>
	using pair = std::pair<const Type, Type>;

	template
	<
		class Iterator
	>
	struct range : pair<Iterator>
	{
		using base = pair<Iterator>;
		using base::base;

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
		class Type, size_t Index,
		template <class> class Sort = std::less
	>
	struct indexical : Sort<Type>
	{
		using base = Sort<Type>

		template <class Tuple>
		bool operator() (Tuple const& x, Type u) const
		{
			return base::operator()(std::get<Index>(x), u);
		}

		template <class Tuple>
		bool operator() (Type u, Tuple const& x) const
		{
			return base::operator()(u, std::get<Index>(x));
		}
	};

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
	using init = std::initializer_list<Type>;

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

	// Directed graphs

	template
	<
		class Type,
		template <class> class Sort = std::less,
		template <class> class Alloc = std::allocator
	>
	using graph = set<pair<Type>, Sort, Alloc>;

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
	auto edges(graph<Type, Sort, Alloc>::const_reference g, Type n)
	{
		indexical<Type, 0, Sort> hat;
		return range<graph<Type, Sort, Alloc>::iterator>
		(
			std::equal_range(begin(g), end(g), n, hat)
		);
	}

	// Strings

	template
	<
		class Char,
		template <class> class Traits = std::char_traits
	>
	using basic_string_view = std::basic_string_view<Char, Traits<Char>>;

	template
	<
		class Char,
		template <class> class Traits = std::char_traits,
		template <class> class Alloc = std::allocator
	>
	using basic_string = std::basic_string<Char, Traits<Char>, Alloc<Char>>;

	// Input/Output

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
}

#endif // file
