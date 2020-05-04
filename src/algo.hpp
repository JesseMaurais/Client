#ifndef algo_hpp
#define algo_hpp "Algorithms"

#include "fmt.hpp"
#include "tmp.hpp"

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
		class Container = Vector<Type, Alloc>,
		class Size = typename Container::size_type
	>
	struct line : pair<Size>
	{
		using base = fwd::pair<Size>;
		using container = Vector<Type, Alloc>;

		container const *that;

		line(container const *ptr, base pos) : that(ptr), base(pos) { }

		line(container const &ref, base pos) : line(&ref, pos) { }

		#ifdef assert
		~line() { assert(this->first <= this->second); }
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
	};

	template
	<
		class Type,
		template <class> class Alloc = allocator,
		template <class, template <class> class> class Vector = vector,
		class Size = typename Vector<Type, Alloc>::size_type
	>
	struct page : line<Type, Alloc, Vector>
	{
		using base = line<Type, Alloc, Vector>;
		line<pair<Size>, Alloc, Vector> index;

		page(decltype(index) in, pair<Size> at)
		: index(in), page(at)
		{ }

		base operator()(auto at) const
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
		static Identity const eq;
		auto const p = std::bind(eq, n);
		return split(s, p, out);
	}

	template
	<
		class Type, 
		template <class> class Alloc = allocator
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
