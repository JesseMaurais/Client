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

	//
	// Run Time Types
	//

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
		class Iterator, int Size = 1, int Step = Size
	>
	struct interval : pair<Iterator>
	// Iterate within a sub range
	{
		static_assert(0 < Size);
		static_assert(0 == Size % Step);

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
		static_assert(First < Last);

		constexpr auto first = First;
		constexpr auto second = Last;

		constexpr auto begin()
		{
			return First;
		}

		constexpr auto end()
		{
			return Last + 1;
		}

		constexpr auto size()
		{
			return end() - begin();
		}

		constexpr auto greater(auto digit)
		{
			ordering<decltype(digit)> const part;
			return part(Last, digit);
		}

		constexpr auto less(auto digit)
		{
			ordering<decltype(digit)> const part;
			return part(digit, First);
		}

		constexpr auto at(auto digit)
		{
			return not greater(digit) and not less(digit);
		}

		constexpr operator range<decltype(first)>()
		{
			return { begin(), end() };
		}
	};

	template
	<
		template <auto, auto> class Part,
		template <auto, auto> class... Parts
	>
	struct compose
	{
		using Rest = compose<Parts...>;

		constexpr auto any(auto digit)
		{
			return Part::at(digit) or ... Rest::any(digit);
		}

		constexpr auto all(auto digit)
		{
			return Part::at(digit) and ... Rest::all(digit);
		}

		constexpr auto size()
		{
			return 1 + sizeof ... Parts;
		}

		constexpr auto begin()
		{
			return Part::first < remainder::begin()
			     ? Part::first : remainder::begin();
		}

		constexpr auto end()
		{
			return Part::second > remainder::end()
			     ? Part::second : remainder::end();
		}

		constexpr operator range<decltype(Part::first)>()
		{
			return { begin(), end() };
		}
	};

	struct
	<
		class Part
	>
	struct compose<Part> : Part
	{
		constexpr auto any(auto digit) { return Part::at(digit); }
		constexpr auto all(auto digit) { return Part::at(digit); }
	};
}

#endif // file
