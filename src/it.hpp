#ifndef it_hpp
#define it_hpp "Iterator Types"

#include "fwd.hpp"

namespace fwd
{
	template
	<
		class Type
	>
	struct iterate : Type
	// Iterator pattern
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
		class Iterator, int Size = 1, int Step = Size,
	>
	struct segment : pair<Iterator>
	// Iterate within a sub range
	{
		using pair::pair;

		struct item : range<Iterator>
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
			return iterate<item>
			{ 
				this->first, std::next(this->first, Size)	
			};
		}

		auto end() const
		{
			return iterate<item>
			{
				std::prev(this->second, Size), this->second;
			};
		}

		static_assert(Size % Step == 0);
		static_assert(0 < Size);
	};

	// Directed graphs

	template
	<
		class Type
	>
	using node = std::pair<Type, span<Type>>;

	template
	<
		class Type,
		template <class> class Order = std::less,
		template <class> class Alloc = std::allocator
	>
	using edges = set<pair<Type>, Order, Alloc>;

	template
	<
		class Type,
		template <class> class Order = std::less,
		template <class> class Alloc = std::allocator
	>
	using graphs = map<Type, edges<Type, Order, Alloc>, Order, Alloc>;
}

#endif // file
