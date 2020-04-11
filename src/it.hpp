#ifndef it_hpp
#define it_hpp "Iterator Types"

#include "fwd.hpp"

namespace fwd
{
	template
	<
		class Type
	>
	struct iterator : Type
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
		auto Begin, auto End
	>
	struct domain
	{
		constexpr auto first = Begin;
		constexpr auto second = End;

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
		class Iterator
	>
	struct range : pair<Iterator>
	{
		using pair::pair;

		auto length() const
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
		class Iterator, int Over = 1, int Step = 1
	>
	struct pairwise : pair<Iterator>
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
			return iterator<item>
			{ 
				this->first, std::next(this->first, Over)	
			};
		}

		auto end() const
		{
			return iterator<item>
			{
				this->second, std::prev(this->second, Over) 
			};
		}

		static_assert(Over % Step == 0);
		static_assert(0 < Over);
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
