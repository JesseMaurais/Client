#ifndef it_hpp
#define it_hpp "Iterator Algorithms"

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

	template <class Base> struct iterate : Base
	{
		using Base::Base;

		auto operator*() const
		{
			return this->value();
		}

		auto operator++()
		{
			this->next();
			return *this;
		}
	};

	template
	<
		class Type, class Pair=pair<Type>
	>
	auto up_to(Type size, Type pos = 0, Type step = 1)
	{
		struct forward : Pair
		{
			using Pair::Pair;

			auto value() const
			{
				return this->first;
			}

			void next()
			{
				this->first += this->second;
			}
		};
		#ifdef assert
		assert(0 == ((size - pos) % step));
		#endif
		return range<iterate<forward>>
		{
			{ pos, step }, { size, step }
		};
	}

	template
	<
		class Type, class Pair=pair<Type>
	>
	auto down_from(Type size, Type pos = 0, Type step = 1)
	{
		struct backward : pair<Type>
		{
			using fwd::pair<Type>::pair;

			auto value() const
			{
				return this->first - 1;
			}

			void next()
			{
				this->first -= this->second;
			}
		};
		#ifdef assert
		assert(0 == ((size - pos) % step));
		#endif
		return range<iterate<backward>>
		{
			{ size, step }, { pos, step }
		};
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

	template <auto First, auto Last> struct interval
	{
		constexpr auto begin() const
		{
			return First;
		}

		constexpr auto end() const
		{
			return Last + 1;
		}

		constexpr auto size() const
		{
			return end() - begin();
		}

		template <class N> constexpr bool greater(N n) const
		{
			return Last < n;
		}

		template <class N> constexpr bool less(N n) const
		{
			return n < First;
		}

		template <class N> constexpr bool any(N n) const
		{
			return not greater(n) and not less(n);
		}
	};

	template <class Part, class... Parts> struct intervals
	{
		using Rest = intervals<Parts...>;

		template <class N> constexpr bool any(N n) const
		{
			return Part::any(n) or Rest::any(n);
		}

		template <class N> constexpr bool all(N n) const
		{
			return Part::any(n) and Rest::all(n);
		}

		constexpr auto size() const
		{
			return 1 + sizeof...(Parts);
		}

		constexpr auto begin() const
		{
			return std::min(Part::begin(), Rest::begin());
		}

		constexpr auto end() const
		{
			return std::max(Part::end(), Rest::end());
		}
	};

	template <class Part> struct intervals<Part>
	{
		template <class N> constexpr bool any(N digit) { return Part::any(digit); }
		template <class N> constexpr bool all(N digit) { return Part::any(digit); }
	};
}

#endif // file
