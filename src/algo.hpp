#ifndef algo_hpp
#define algo_hpp "Algorithms"

#include "fwd.hpp"
#include "tmp.hpp"
#include "ptr.hpp"
#include <utility>
#include <tuple>

namespace fwd
{
	//
	// Data Matrix
	//

	template <class... Columns> class matrix
	{
		using tuple = std::tuple<vector<Columns>...>;
		using index = std::make_index_sequence<std::tuple_size<tuple>::value>;

		template <size_t... Count>
		auto data(std::index_sequence<Count...>) const
		{
			return std::make_tuple(std::get<Count>(table).data()...);
		}

		template <size_t... Count>
		auto size(std::index_sequence<Count...>) const
		{
			return std::make_tuple(std::get<Count>(table).size()...);
		}

		template <size_t... Count>
		auto empty(std::index_sequence<Count...>) const
		{
			return std::make_tuple(std::get<Count>(table).empty()...);
		}

		template <size_t... Count>
		auto at(size_t row, std::index_sequence<Count...>)
		{
			return std::forward_as_tuple(std::get<Count>(table).at(row)...);
		}

		template <size_t... Count>
		auto back(std::index_sequence<Count...>)
		{
			return std::forward_as_tuple(std::get<Count>(table).back()...);
		}

		template <size_t... Count>
		auto emplace_back(Columns... row, std::index_sequence<Count...>)
		{
			return std::forward_as_tuple(std::get<Count>(table).emplace_back(row)...);
		}

		template <size_t... Count>
		void pop_back(std::index_sequence<Count...>)
		{
			(std::get<Count>(table).pop_back(), ...);
		}

		template <size_t... Count>
		void swap(size_t i, size_t j, std::index_sequence<Count...>)
		{
			(std::swap(std::get<Count>(table).at(i), std::get<Count>(table).at(j)), ...);
		}

		template <size_t... Count>
		void resize(size_t n, std::index_sequence<Count...>)
		{
			(std::get<Count>(table).resize(n), ...);
		}

		tuple table;

	public:

		auto data() const
		{
			return data(index());
		}

		auto size() const
		{
			return size(index());
		}

		auto empty() const
		{
			return empty(index());
		}

		auto at(size_t row)
		{
			return at(row, index());
		}

		auto back()
		{
			return back(index());
		}

		auto emplace_back(Columns... row)
		{
			return emplace_back(row..., index());
		}

		void pop_back()
		{
			pop_back(index());
		}

		void swap(size_t i, size_t j)
		{
			swap(i, j, index());
		}

		void resize(size_t n)
		{
			resize(index());
		}
	};

	//
	// Graph Structure
	//

	template <class Node> using edges = std::pair<const Node, span<Node>>;

	template
	<
		class Node, template <class> class Alloc = allocator
	>
	using graph = vector<pair<Node>, Alloc>;

	template
	<
		class Node, template <class> class Alloc = allocator, template <class> class Order = order
	>
	using group = std::map<pair<Node>, Node, Order<pair<Node>>, Alloc<std::pair<const pair<Node>, Node>>>;

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
	bool all_of(Range &&r, Predicate p)
	{
		return std::all_of(r.begin(), r.end(), p);
	}

	template
	<
		class Range, class Predicate
	>
	bool any_of(Range &&r, Predicate p)
	{
		return std::any_of(r.begin(), r.end(), p);
	}

	template
	<
		class Range, class Predicate
	>
	auto find_if(Range &&r, Predicate p)
	{
		return std::find_if(r.begin(), r.end(), p);
	}

	template
	<
		class Range, class Offset, class Predicate
	>
	auto find_if(Range &&r, Offset off, Predicate p)
	{
		return find_if(r, [=](auto const &obj)
		{
			return p(obj.*off);
		});
	}

	template <class Type> inline auto equal_to(Type right)
	{
		return [right](Type left)
		{
			return left == right;
		};
	}

	template <class Range> bool all(Range&& range, bool value = true)
	{
		return all_of(range, equal_to(value));
	}

	template <class Range> bool any(Range&& range, bool value = true)
	{
		return any_of(range, equal_to(value));
	}

	template <class Range> bool uniform(Range&& range)
	{
		return all_of(range, equal_to(range.front()));
	}

	//
	// Container View
	//

	template
	<
		class Type
		,
		class View = span<Type>
		,
		class Container = vector<Type>
		,
		class Pointer = typename Container::const_pointer
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
}

#endif // file
