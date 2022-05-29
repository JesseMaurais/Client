#ifndef fwd_hpp
#define fwd_hpp "Forward Declarations"

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
#include <tuple>
#include <string>
#include <sstream>
#include <string_view>
#include <locale>

namespace fwd
{
	template <class T, class S=T> using pair = std::pair<T, S>;
	template <class Type> using init = std::initializer_list<Type>;
	template <class Type, size_t Size = std::dynamic_extent>
	using span = std::span<Type, Size>;

	template
	<
		class Type, size_t Size
	>
	using array = std::array<Type, Size>;

	template
	<
		class Type, template <class> class Alloc = std::allocator
	>
	using vector = std::vector<Type, Alloc<Type>>;

	template
	<
		class Type
		,
		template <class> class Order = std::less
		,
		template <class> class Alloc = std::allocator
	>
	using set = std::set<Type, Order<Type>, Alloc<Type>>;

	template
	<
		class Key
		,
		class Value
		,
		template <class> class Order = std::less
		,
		template <class> class Alloc = std::allocator
	>
	using map = std::map<Key, Value, Order<Key>, Alloc<std::pair<const Key, Value>>>;

	template
	<
		class Char, template <class> class Traits = std::char_traits
	>
	using basic_string_view = std::basic_string_view<Char, Traits<Char>>;

	template
	<
		class Char
		,
		template <class> class Traits = std::char_traits
		,
		template <class> class Alloc = std::allocator
	>
	using basic_string = std::basic_string<Char, Traits<Char>, Alloc<Char>>;

	template
	<
		class Char, template <class> class Traits = std::char_traits
	>
	using basic_ios = std::basic_ios<Char, Traits<Char>>;

	template
	<
		class Char, template <class> class Traits = std::char_traits
	>
	using basic_istream = std::basic_istream<Char, Traits<Char>>;

	template
	<
		class Char, template <class> class Traits = std::char_traits
	>
	using basic_iostream = std::basic_iostream<Char, Traits<Char>>;

	template
	<
		class Char, template <class> class Traits = std::char_traits
	>
	using basic_ostream = std::basic_ostream<Char, Traits<Char>>;

	template
	<
		class Char, template <class> class Traits = std::char_traits
	>
	using basic_buf = std::basic_streambuf<Char, Traits<Char>>;

	template
	<
		class Char, template <class> class Traits = std::char_traits
	>
	using basic_file = std::basic_filebuf<Char, Traits<Char>>;

	template
	<
		class Char
		,
		template <class> class Traits = std::char_traits
		,
		template <class> class Alloc = std::allocator
	>
	using basic_stringstream = std::basic_stringstream<Char, Traits<Char>, Alloc<Char>>;
}

#endif // file
