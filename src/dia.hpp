#ifndef dia_hpp
#define dia_hpp

#include "fwd.hpp"
#include "tmp.hpp"

namespace fwd
{
	// Structures
	
	template
	<
		class Node
	>
	using edges = std::pair<const Node, span<Node>>;

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
	using group = map<Type, span<pair<Node>>, Order, Alloc>;

	// Algorithms

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
 		class Type, template <class> class Alloc = allocator
	>
	auto split(span<Type> s, predicate<Type> p = std::empty<Type>)
	// Partition a span by (empty) predicate
	{
		vector<Type, Alloc> out;
		auto const begin = std::back_inserter(out);
		auto const end = split(s, op, begin);
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
