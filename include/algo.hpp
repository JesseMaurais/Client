#ifndef algo_hpp
#define algo_hpp "Algorithms"

#include "fwd.hpp"
#include "tmp.hpp"
#include "ptr.hpp"

namespace fwd
{
	template <class Iterator> struct range : pair<Iterator>
	{
		using pair<Iterator>::pair;

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

		template <class N> bool greater(N n) const
		{
			return this->second <= n;
		}

		template <class N> bool less(N n) const
		{
			return n < this->first;
		}

		template <class N> bool any(N n) const
		{
			return not greater(n) and not less(n);
		}
	};

	template
	<
		class Type, class Iterator
	>
	auto split(span<Type> s, predicate<Type> p, Iterator out)
	{
		auto begin = s.begin();
		const auto end = s.end();
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
		template <class> class Alloc = std::allocator
		,
		template <class, template<class> class> class Vector = vector
	>
	auto split(span<Type> s, predicate<Type> p = std::empty<Type>)
	{
		Vector<Type, Alloc> out;
		auto const begin = std::back_inserter(out);
		auto const end = split(s, p, begin);
		return out;
	}

	template
	<
		class Type, class Iterator, template<class> class Identity = std::equal_to
	>
	auto split(span<Type> s, Type n, Iterator out)
	{
		static Identity const q;
		auto const p = std::bind(q, n);
		return split(s, p, out);
	}

	template
	<
		class Type, template <class> class Alloc = std::allocator
	>
	auto split(span<Type> s, Type n = { })
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
		class Type, template <class> class Alloc = std::allocator
	>
	auto join(span<span<Type>> s, Type n = { })
	{
		vector<Type, Alloc> out;
		auto const begin = std::back_inserter(out);
		auto const end = join(s, n, begin);
		return out;
	}

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

	template
	<
		class Type
		,
		template <class> class Alloc = std::allocator
		,
		class View = span<Type>
		,
		class Container = vector<Type, Alloc>
		,
		class Pointer = as_ptr<Container>
		,
		class Size = typename Container::size_type
		,
		class Base = pair<Size>
	>
	class page : Base
	{
		Pointer that;

	public:

		page(Size begin, Size end, Pointer ptr)
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

	template
	<
		class Char
		,
		template <class> class Traits = std::char_traits
		,
		template <class> class Alloc = std::allocator
		,
		class View = basic_string_view<Char, Traits>
		,
		class Container = basic_string<Char, Traits, Alloc>
		,
		class Pointer = as_ptr<Container>
		,
		class Size = typename Container::size_type
		,
		class Base = pair<Size>
	>
	using line = page<Char, Alloc, View, Container, Pointer, Size, Base>;
}

#endif // file
