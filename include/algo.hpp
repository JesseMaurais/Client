#ifndef algo_hpp
#define algo_hpp "Algorithms"

#include "fwd.hpp"
#include "tmp.hpp"
#include "ptr.hpp"
#include <iterator>

namespace fwd
{
	template <class It> struct range : pair<It>
	{
		using pair = pair<It>;
		using pair::pair;

		bool less(It it) const
		{
			return it < pair::first;
		}

		bool greater(It it) const
		{
			return pair::second < it;
		}

		bool excludes(It it) const
		{
			return greater(it) or less(it);
		}

		bool contains(It it) const
		{
			return not excludes(it);
		}

		auto empty() const
		{
			return pair::first == pair::second;
		}

		auto diff() const
		{
			return std::distance(pair::first, pair::second);
		}

		auto begin() const
		{
			return pair::first;
		}

		auto end() const
		{
			return pair::second;
		}

		auto rbegin() const
		{
			return std::reverse_iterator(begin());
		}

		auto rend() const
		{
			return std::reverse_iterator(rend());
		}

		auto reverse() const
		{
			return range(rbegin(), rend());
		}
	};

	template <class Type> auto reverse(span<Type> s)
	{
		return range(s.rbegin(), s.rend());
	}

	template
	<
		class Type, class It
	>
	auto split(span<Type> s, predicate<Type> p, It out)
	{
		auto begin = s.begin();
		const auto end = s.end();
		for (auto it = begin; it != end; ++it)
		{
			if (p(*it))
			{
				++ out = Type(begin, it);
				begin = std::next(it);
			}
		}
		return out;
	}

	template <class Type> auto empty(Type it) { return std::empty(it); }

	template
	<
 		class Type
		,
		template <class> class Alloc = std::allocator
		,
		template <class, template<class> class> class Vector = vector
	>
	auto split(span<Type> s, predicate<Type> p = empty<Type>)
	{
		Vector<Type, Alloc> out;
		auto it = std::back_inserter(out);
		(void) split(s, p, it);
		return out;
	}

	template
	<
		class Type, class It, template<class> class Id = std::equal_to
	>
	auto split(span<Type> s, Type tok, It it)
	{
		constexpr Id id;
		auto p = std::bind(id, tok);
		return split(s, p, it);
	}

	template
	<
		class Type, template <class> class Alloc = std::allocator
	>
	auto split(span<Type> s, Type tok = { })
	{
		vector<Type, Alloc> out;
		auto it = std::back_inserter(out);
		(void) split(s, tok, it);
		return out;
	}

	template
	<
		class Type, class It
	>
	auto join(span<span<Type>> s, Type tok, It it)
	{
		for (auto i : s)
		{
			for (auto j : i)
			{
				++it = j;
			}
			++it = tok;
		}
		return it;
	}

	template
	<
		class Type, template <class> class Alloc = std::allocator
	>
	auto join(span<span<Type>> s, Type tok = { })
	{
		vector<Type, Alloc> out;
		auto it = std::back_inserter(out);
		(void) join(s, tok, it);
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
}

#endif // file
