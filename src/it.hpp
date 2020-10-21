#ifndef it_hpp
#define it_hpp "Iterator Types"

#include "fwd.hpp"
#include "tmp.hpp"

namespace fwd
{
	template
	<
		class Base
	>
	struct iterate : Base
	// Iterator wrapper
	{
		using Base::Base;

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

	//
	// Run Time Types
	//

	template
	<
		class Iterator, class Base = pair<Iterator>
	>
	struct range : Base
	// Iterator pair as range
	{
		using Base::Base;

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
			thread_local order<N> const sorted;
			return not sorted(n, this->second);
		}

		template <class N> bool less(N n) const
		{
			thread_local order<N> const sorted;
			return sorted(n, this->first);
		}

		template <class N> bool at(N n) const
		{
			return not greater(n) and not less(n);
		}
	};

	template
	<
		class Iterator, 
		int Size = 1, 
		int Step = Size, 
		// details
		class Range = range<Iterator>,
		class Base = pair<Iterator>
	>
	struct interval : Base
	// Iterate within a sub range
	{
		static_assert(0 < Size);
		static_assert(0 == Size % Step);

		using Base::Base;

		struct iterator : Range
		{
			using Range::Range;

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
				std::prev(this->second, Size), this->second
			};
		}
	};

	//
	// Compile Time Expressions
	//

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
