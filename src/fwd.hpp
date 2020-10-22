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
#include <tuple>
#include <string>
#include <string_view>

namespace fwd
{
	// Class union

	template <class... Types> struct compose : virtual Types... { };

	// Overloadable traits

	template <class Type> using allocator = std::allocator<Type>;
	template <class Type> using iterator = std::iterator_traits<Type>;
	template <class Type> using order = std::less<Type>;
	template <class Type> using identity = std::equal_to<Type>;
	template <class Char> using character = std::char_traits<Char>;

	// Containers

	template <class Type> using pair = std::pair<const Type, Type>;
	template <class Type> using init = std::initializer_list<Type>;
	template <class Type> using span = std::span<Type>;

	template
	<
		class Type, size_t Size
	>
	using array = std::array<Type, Size>;

	template 
	<
		class Type, template <class> class Alloc = allocator
	>
	using vector = std::vector<Type, Alloc<Type>>;

	template 
	<
		class Type
		,
		template <class> class Order = order
		,
		template <class> class Alloc = allocator
	>
	using set = std::set<Type, Order<Type>, Alloc<Type>>;

	template 
	<
		class Type
		,
		template <class> class Order = order
		,
		template <class> class Alloc = allocator
	>
	using map = std::map<Type, Type, Order<Type>, Alloc<pair<Type>>>;

	//
	// Strings
	//

	template
	<
		class Char, template <class> class Traits = character
	>
	using basic_string_view = std::basic_string_view<Char, Traits<Char>>;

	template
	<
		class Char
		,
		template <class> class Traits = character
		,
		template <class> class Alloc = allocator
	>
	using basic_string = std::basic_string<Char, Traits<Char>, Alloc<Char>>;

	//
	// Input/Output
	//

	template
	<
		class Char, template <class> class Traits = character
	>
	using basic_ios = std::basic_ios<Char, Traits<Char>>;

	template
	<
		class Char, template <class> class Traits = character
	>
	using basic_istream = std::basic_istream<Char, Traits<Char>>;

	template
	<
		class Char, template <class> class Traits = character
	>
	using basic_iostream = std::basic_iostream<Char, Traits<Char>>;

	template
	<
		class Char, template <class> class Traits = character
	>
	using basic_ostream = std::basic_ostream<Char, Traits<Char>>;

	template
	<
		class Char, template <class> class Traits = character
	>
	using basic_buf = std::basic_streambuf<Char, Traits<Char>>;

	template
	<
		class Char, template <class> class Traits = character
	>
	using basic_file = std::basic_filebuf<Char, Traits<Char>>;

	template
	<
		class Char
		,
		template <class> class Traits = character
		,
		template <class> class Alloc = allocator
	>
	using basic_stringstream = std::basic_stringstream<Char, Traits<Char>, Alloc<Char>>;
}

#endif // file
