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

	template <class Type> auto up_to(Type size, Type pos = 0, Type step = 1)
	{
		struct forward : pair<Type>
		{
			using fwd::pair<Type>::pair;

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
			using fwd::pair<Type>::pair;

			auto value() const
			{
				return this->second - 1;
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
			{ step, size }, { step, pos }
		};
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
