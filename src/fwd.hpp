#ifndef fwd_hpp
#define fwd_hpp "Forward Declaration"

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
#include <tuple>

namespace fwd
{
	// Containers

	template 
	<
		class Type
	>
	using pair = std::pair<const Type, Type>;

	template
	<
		class Type,
		template <class> class Order = std::less,
		template <class> class Alloc = std::allocator
	>
	using set = std::set<Type, Order<Type>, Alloc<Type>>;

	template 
	<
		class Type,
		template <class> class Order = std::less
		template <class> class Alloc = std::allocator
	>
	using map = std::map<Type, Type, Order<Type>, Alloc<pair<Type>>;

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
	using basic_file = std::basic_filebuf<Char, Traits<Char>>;
}

#endif // file
