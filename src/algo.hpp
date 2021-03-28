#ifndef algo_hpp
#define algo_hpp "Algorithms"

#include "fwd.hpp"
#include "tmp.hpp"
#include "ptr.hpp"

namespace fwd
{
	//
	// Structured Text
	//

	template
	<
		class Type
		,
		class View = span<Type>
		,
		class Container = vector<Type>
		,
		class Pointer = as_ptr<const Container>
		,
		class Size = typename Container::size_type
		,
		class Base = pair<Size>
	>
	class line : Base
	{
		Pointer that;

	public:

		line(Size begin, Size end, Pointer ptr) 
		: Base(begin, end), that(ptr)
		{ }

		bool empty() const { return this->second == this->first; }

		auto size() const { return this->second - this->first; }

		auto begin() const { return that->data() + this->first; }

		auto end() const { return that->data() + this->second; }

		operator View() const
		{
			return { begin(), end() };
		}

		auto operator[](std::ptrdiff_t index) const 
		{
			#ifdef assert
			assert(size() > index)
			assert(index > -1);
			#endif
			return that->at(this->first + index);
		}
	};

	//
	// Directed Graph
	//

	template <class Node> using edges = std::pair<Node, span<Node>>;

	template
	<
		class Node, template <class> class Alloc = allocator
	>
	using graph = std::vector<pair<Node>, Alloc<pair<Node>>>;

	template
	<
		class Node
		, 
		template <class> class Order = order
		,
		template <class> class Alloc = allocator
	>
	using group = std::map<pair<Node>, Node, Order<pair<Node>>, Alloc<pair<Node>>>;

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
 		class Type
		, 
		template <class> class Alloc = allocator
		,
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
		class Type, class Iterator, template<class> class Identity = identity
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
		class Type, template <class> class Alloc = allocator
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

	//
	// Ranges
	//

	template
	<
		class Range, class Predicate
	>
	inline bool all_of(Range&& range, Predicate&& check)
	{
		return std::all_of(range.begin(), range.end(), check);
	}

	template
	<
		class Range, class Predicate
	>
	inline bool any_of(Range&& range, Predicate&& check)
	{
		return std::any_of(range.begin(), range.end(), check);
	}
}

#endif // file
