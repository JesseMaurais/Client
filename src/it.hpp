#ifndef it_hpp
#define it_hpp "Iterator Types"

#include "fwd.hpp"

namespace fwd
{
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
			order<N> const sorted;
			return not sorted(n, this->second);
		}

		template <class N> bool less(N n) const
		{
			order<N> const sorted;
			return sorted(n, this->first);
		}

		template <class N> bool at(N n) const
		{
			return not greater(n) and not less(n);
		}
	};

	template <class Type> auto up_to(Type size, Type pos = 0, Type step = 1)
	{
		struct forward : pair<Type>
		{
			using pair<Type>::pair;

			auto value() const
			{
				return this->second;
			}

			void next()
			{
				this->second += this->first;
			}
		};
		#ifdef assert
		assert(0 == ((size - pos) % step));
		#endif
		return range<iterate<forward>>
		{
			{ step, pos }, { step, size }
		};
	}

	template <class Type> auto down_from(Type size, Type pos = 0, Type step = 1)
	{
		struct backward : pair<Type>
		{
			using pair<Type>::pair;

			auto value() const
			{
				return this->second;
			}

			void next()
			{
				this->second -= this->first;
			}
		};
		#ifdef assert
		assert(0 == ((size - pos) % step));
		#endif
		return range<iterate<backward>>
		{
			{ step, size-1 }, { step, pos-1 }
		};
	}

	// Borel sets

	template <auto First, auto Last> struct closure
	{
		static_assert(First < Last + 1);

		static constexpr auto first = First;
		static constexpr auto second = Last;

		constexpr auto begin() const
		{
			return first;
		}

		constexpr auto end() const
		{
			return second + 1;
		}

		constexpr auto size() const
		{
			return end() - begin();
		}

		template <class N> constexpr bool less(N digit) const
		{
			return digit < first;
		}

		template <class N> constexpr bool more(N digit) const
		{
			return second < digit;
		}

		template <class N> constexpr bool over(N digit) const
		{
			return not more(digit) and not less(digit);
		}
	};

	template <class Part, class... Parts> struct closures
	{
		using Rest = closures<Parts...>;

		template <class N> constexpr bool any(N digit) const
		{
			return Part::over(digit) or Rest::any(digit);
		}

		template <class N> constexpr bool all(N digit) const
		{
			return Part::over(digit) and Rest::all(digit);
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

	template
	<
		class Part
	>
	struct closures<Part>
	{
		template <class N> constexpr bool any(N digit) { return Part::over(digit); }
		template <class N> constexpr bool all(N digit) { return Part::over(digit); }
	};
}

#endif // file
