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
	template <class... Types> struct compose : Types... { };

	// Default Traits

	template
	<
		class Type
	>
	using allocator = std::allocator<Type>;

	template
	<
		class Type
	>
	using iterator = std::iterator_traits<Type>;

	template
	<
		class Char
	>
	using character = std::char_traits<Type>;

	template
	<
		class Type
	>
	using ordering = std::less<Type>;

	template
	<
		class Type
	>
	using identity = std::equal_to<Type>;

	// Containers

	template 
	<
		class Type
	>
	using pair = std::pair<const Type, Type>;

	template
	<
		class Type,
		template <class> class Order = ordering,
		template <class> class Alloc = allocator
	>
	using set = std::set<Type, Order<Type>, Alloc<Type>>;

	template 
	<
		class Type,
		template <class> class Order = ordering,
		template <class> class Alloc = allocator
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
		template <class> class Alloc = allocator
	>
	using vector = std::vector<Type, Alloc<Type>>;

	template
	<
		class Type, size_t Size,
		template <class> class Alloc = allocator
	>
	using variant = std::variant<array<Type, Size>, vector<Type, Alloc>>;

	//
	// Strings
	//

	template
	<
		class Char,
		template <class> class Traits = character
	>
	using basic_string_view = std::basic_string_view<Char, Traits<Char>>;

	template
	<
		class Char,
		template <class> class Traits = character,
		template <class> class Alloc = allocator
	>
	using basic_string = std::basic_string<Char, Traits<Char>, Alloc<Char>>;

	//
	// Input/Output
	//

	template
	<
		class Char,
		template <class> class Traits = character
	>
	using basic_ios = std::basic_ios<Char, Traits<Char>>;

	template
	<
		class Char,
		template <class> class Traits = character
	>
	using basic_istream = std::basic_istream<Char, Traits<Char>>;

	template
	<
		class Char,
		template <class> class Traits = character
	>
	using basic_iostream = std::basic_iostream<Char, Traits<Char>>;

	template
	<
		class Char,
		template <class> class Traits = character
	>
	using basic_ostream = std::basic_ostream<Char, Traits<Char>>;

	template
	<
		class Char,
		template <class> class Traits = character
	>
	using basic_buf = std::basic_streambuf<Char, Traits<Char>>;

	template
	<
		class Char
		template <class> class Traits = character
	>
	using basic_file = std::basic_filebuf<Char, Traits<Char>>;

	//
	// Structured Text
	//

	template
	<
		class Type, 
		template <class> class Alloc = allocator,
		template <class, class> class Vector = vector
	>
	struct line : pair<const Vector<Type, Alloc>::size_type>
	{
		using Vector<Type, Alloc>::const_pointer;
		line(const_pointer ptr, pair pos) : pair(pos), that(ptr) { }

		using Vector<Type, Alloc>::const_reference;
		line(const_reference ref, pair pos) : line(&ref, pos) { }

		#ifdef assert
		~line() { assert(this->first < this->second); }
		#endif

		auto size() const { return this->second - this->first; }

		auto begin() const { return that->data() + this->first; }

		auto end() const { return that->data() + this->second; }

		auto operator[](auto index) const 
		{ 
			return that->at(this->first + index);
		}

		operator span<Type>() const
		{
			return { begin(), end() };
		}

		const_pointer that;
	};

	template
	<
		class Type,
		template <class> class Alloc = allocator,
		template <class, class> class Vector = vector
	>
	struct page : line<Type, Alloc, Vector>
	{
		using line<Type, Alloc, Vector>::first_type;
		line<pair<first_type>, Alloc, Vector> index;

		page(decltype(index) in, pair<first_type> at)
		: index(in), page(at)
		{ }

		line operator()(auto at) const
		{
			return { this->that, index[at] };
		}
	};

	//
	// Directed Graph
	//

	template
	<
		class Node
	>
	using edges = std::pair<const Node, line<Node>>;

	template
	<
		class Node, 
		template <class> class Alloc = allocator
	>
	using graph = vector<pair<Node>, Alloc>;

	template
	<
		class Node, 
		template <class> class Order = ordering, 
		template <class> class Alloc = allocator
	>
	using group = map<type, line<pair<Node>>, Order, Alloc>;

	//
	// Algorithms
	//

	template
	<
		class Type, class Iterator
	>
	auto split(span<Type> s, predicate<Type> p, Iterator out)
	// Partition a span by predicate
	{
		auto begin = s.begin();
		auto const end = s.end();
		for (auto it = begin; it != end; ++it)
		{
			if (p(*it))
			{
				++out = Type(begin, it);
				begin = std::next(it);
			}
		}
		return out;
	}

	template 
	<
 		class Type, 
		template <class> class Alloc = allocator,
		template <class, class> class Vector = vector
	>
	auto split(span<Type> s, predicate<Type> p = std::empty<Type>)
	// Partition a span by (empty) predicate
	{
		Vector<Type, Alloc> out;
		auto const begin = std::back_inserter(out);
		auto const end = split(s, p, begin);
		return out;
	}

	template
	<
		class Type, class Iterator, class Identity = identity
	>
	auto split(span<Type> s, Type n, Iterator out)
	// Partition a span by value
	{
		static Identity const eq;
		auto const p = std::bind(eq, n);
		return split(s, p, out);
	}

	template
	<
		class Type, template <class> class Alloc = allocator
	>
	auto split(span<Type> s, Type n)
	// Partition a span by value
	{
		vector<Type, Alloc> out;
		auto const begin = std::back_inserter(out);
		auto const end = split(s, n, begin);
		return out;
	}

	template
	<
		class Type, class Iterator
	>
	auto join(span<span<Type>> s, Type n, Iterator out)
	// Join spans with separator
	{
		for (auto i : s)
		{
			for (auto j : i)
			{
				++out = j;
			}
			++out = n;
		}
		return out;
	}

	template
	<
		class Type, template <class> class Alloc = allocator
	>
	auto join(span<span<Type>> s, Type n = { })
	// Join spans with separator
	{
		vector<Type, Alloc> out;
		auto const begin = std::back_inserter(out);
		auto const end = join(s, n, begin);
		return out;
	}
}

#endif // file
