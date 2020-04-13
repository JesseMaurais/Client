#ifndef it_hpp
#define it_hpp "Iterator Types"

#include "fwd.hpp"
#include "tmp.hpp"

namespace fwd
{
	template
	<
		class Type
	>
	struct iterate : Type
	// Iterator wrapper
	{
		using Type::Type;

		auto operator*() const
		{
			return *this;
		}

		auto operator++()
		{
			this->next();
			return *this;
		}
	};

	template
	<
		class Iterator
	>
	struct range : pair<Iterator>
	// Iterator pair as range
	{
		using pair::pair;

		auto size() const
		{
			return std::distance(this->first, this->second);
		}

		auto begin() const
		{
			return this->first;
		}

		auto end() const
		{
			return this->second;
		}
	};

	template
	<
		auto Begin, auto End
	>
	struct domain
	// Constant range expression
	{
		constexpr auto first = Begin;
		constexpr auto second = End;

		static_assert(first < second);

		constexpr auto size()
		{
			return second - first;
		}

		constexpr auto begin()
		{
			return first;
		}

		constexpr auto end()
		{
			return second;
		}
	};

	template
	<
		class Iterator, int Size = 1, int Step = Size
	>
	struct interval : pair<Iterator>
	// Iterate within a sub range
	{
		using pair::pair;

		struct iterator : range<Iterator>
		{
			using range::range;

			void next()
			{
				std::advance(this->first, Step);
				std::advance(this->second, Step);
			}
		};

		auto begin() const
		{
			return iterate<iterator>
			{ 
				this->first, std::next(this->first, Size)	
			};
		}

		auto end() const
		{
			return iterate<iterator>
			{
				std::prev(this->second, Size), this->second;
			};
		}

		static_assert(Size % Step == 0);
		static_assert(0 < Size);
	};

	//
	// Structures
	//

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
