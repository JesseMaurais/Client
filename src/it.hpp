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

		auto greater(auto digit) const
		{
			ordering<decltype(digit)> const part;
			return not part(digit, this->second);
		}

		auto less(auto digit) const
		{
			ordering<decltype(digit)> const part;
			return part(digit, this->first);
		}

		auto at(auto digit) const
		{
			return not greater(digit) and not less(digit);
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

	template
	<
		auto First, auto Last
	>
	struct closure
	{
		static_assert(First < Last + 1);

		static constexpr auto first = First;
		static constexpr auto second = Last;

		constexpr auto begin()
		{
			return first;
		}

		constexpr auto end()
		{
			return second + 1;
		}

		constexpr auto size()
		{
			return end() - begin();
		}

		constexpr auto less(auto digit)
		{
			return digit < first;
		}

		constexpr auto more(auto digit)
		{
			return second < digit;
		}

		constexpr auto over(auto digit)
		{
			return not more(digit) and not less(digit);
		}
	};

	template
	<
		class Part, class... Parts
	>
	struct closures
	{
		using Rest = closures<Parts...>;

		constexpr auto any(auto digit)
		{
			return Part::over(digit) or Rest::any(digit);
		}

		constexpr auto all(auto digit)
		{
			return Part::over(digit) and Rest::all(digit);
		}

		constexpr auto size()
		{
			return 1 + sizeof...(Parts);
		}

		constexpr auto begin()
		{
			return std::min(Part::begin(), Rest::begin());
		}

		constexpr auto end()
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
		constexpr auto any(auto digit) { return Part::over(digit); }
		constexpr auto all(auto digit) { return Part::over(digit); }
	};
}

#endif // file
