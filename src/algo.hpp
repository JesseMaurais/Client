#ifndef algo_hpp
#define algo_hpp "Algorithms"

#include "fmt.hpp"
#include "tmp.hpp"
#include <memory>

namespace fwd
{
	//
	// Structured Text
	//

	template
	<
		class Type,
		template <class> class Alloc = allocator,
		template <class, template<class> class> class Vector = vector,
		template <class> class Span = span,
		// details
		class View = Span<Type>,
		class Container = Vector<Type, Alloc>,
		class Size = typename Container::size_type,
		class Pointer = typename Container::const_pointer,
		class Reference = typename Container::const_reference,
		class Base = pair<Size>
	>
	struct line : Base
	{
		Pointer that;

		line(Base pos, Pointer ptr) 
		: Base(pos), that(ptr)
		{ }

		line(Base pos, Reference ref) 
		: line(pos, &ref) 
		{ }

		auto size() const { return this->second - this->first; }

		auto begin() const { return that->data() + this->first; }

		auto end() const { return that->data() + this->second; }

		#ifdef assert
		~line() { assert(this->first <= this->second); }
		#endif

		operator View() const
		{
			return { begin(), end() };
		}

		auto operator[](auto index) const 
		{
			return that->at(this->first + index);
		}
	};

	template
	<
		class Type,
		template <class> class Alloc = allocator,
		template <class, template <class> class> class Vector = vector,
		template <class> class Span = span,
		// details
		class View = Span<Type>,
		class Container = Vector<Type, Alloc>,
		class Size = typename Container::size_type,
		class Pair = pair<Size>,
		class Index = line<Pair, Alloc, Vector, Span>,
		class Base = line<Type, Alloc, Vector, Span>
	>
	struct page : Base
	{
		Index index;

		page(Base pos, Index in) 
		: Base(pos), index(in)
		{ }

		Base operator()(auto at) const
		{
			return { index[at], this->that };
		}
	};

	//
	// Directed Graph
	//

	template
	<
		class Node,
		template <class> class Alloc = allocator,
		template <class, template <class> class> class Vector = vector,
		template <class> class Span = span,
		// details
		class List = line<Node, Alloc, Vector, Span>
	>
	using edges = std::pair<const Node, List>;

	template
	<
		class Node, 
		template <class> class Alloc = allocator
	>
	using graph = std::vector<pair<Node>, Alloc<Node>>;

	template
	<
		class Node, 
		template <class> class Order = ordering,
		template <class> class Alloc = allocator
	>
	using group = std::map<pair<Node>, Node, Order<Node>, Alloc<Node>>;

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
		template <class, template<class> class> class Vector = vector
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
		class Type, 
		class Iterator, 
		template<class> class Identity = identity
	>
	auto split(span<Type> s, Type n, Iterator out)
	// Partition a span by value
	{
		static Identity const q;
		auto const p = std::bind(q, n);
		return split(s, p, out);
	}

	template
	<
		class Type, 
		template <class> class Alloc = allocator
	>
	auto split(span<Type> s, Type n = { })
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
