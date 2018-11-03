#ifndef top_hpp
#define top_hpp

// Topology toolset based on a reading of James Munkres' book.

#include <cmath>
#include <cassert>
#include <utility>
#include <variant>
#include <stdexcept>
#include <functional>
#include <type_traits>
#include "tool.hpp"

namespace top
{
	template <template <typename> typename Bound, typename Value>
	struct order : stl::not_copyable
	{
		using value_type = Value;
		using boundary_type = Bound<value_type>;

	private:

		using less_than = std::less<value_type>;
		using greater_than = std::greater<value_type>;
		using not_less_than = std::greater_equal<value_type>;
		using not_greater_than = std::less_equal<value_type>;
		using equal_to = std::equal_to<value_type>;
		using not_equal_to = std::not_equal_to<value_type>;
		using less_than_or_equal_to = not_greater_than;
		using greater_than_or_equal_to = not_less_than;

		template <class C>
		static constexpr bool is_base_of = std::is_base_of_v<C, boundary_type>;

		template <class A, class B, class C>
		static constexpr bool is_base_one_of = is_base_of<A> or is_base_of<B> or is_base_of<C>;

	public:

		static constexpr bool is_open = is_base_one_of<less_than, greater_than, not_equal_to>;
		static constexpr bool is_closed = is_base_one_of<not_less_than, not_greater_than, equal_to>;
		static constexpr bool is_bounded_above = is_base_one_of<less_than, not_greater_than, equal_to>;
		static constexpr bool is_bounded_below = is_base_one_of<greater_than, not_less_than, equal_to>;

		const value_type &limit;
		const boundary_type compare;

		explicit order(const value_type &ref) : limit(ref), compare()
		{
			if (std::isinf(limit)) throw std::out_of_range("infinity");
		}

		bool operator==(const order &that) const
		{
			return limit == that.limit;
		}

		bool contains(const value_type &x) const
		{
			return compare(x, limit);
		}

		bool operator()(const value_type &x) const
		{
			return contains(x);
		}

		template <template <typename> typename R, typename V>
		bool is_subset_of(const order<R, V> &that) const
		{
			using other = decltype(that);

			if constexpr (other::is_bounded_above != is_bounded_above)
			{
				return false;
			}
			else
			if constexpr (other::is_bounded_below != is_bounded_below)
			{
				return false;
			}
			else
			if constexpr (other::is_bounded_above and other::is_bounded_below)
			{
				return *this == that;
			}
			else
			{
				return that.contains(limit);
			}
		}

		template <template <typename> typename R, typename V>
		bool operator<(const order<R, V> &that) const
		{
			return is_subset_of(that);
		}
	};

	//
	// Alias Types
	//

	template <typename Value> using greater_than = order<std::greater, Value>;
	template <typename Value> using not_greater_than = order<std::less_equal, Value>;
	template <typename Value> using less_than = order<std::less, Value>;
	template <typename Value> using not_less_than = order<std::greater_equal, Value>;
	template <typename Value> using equal_to = order<std::equal_to, Value>;
	template <typename Value> using not_equal_to = order<std::not_equal_to, Value>;

	// equivalents

	template <typename Value> using less_than_or_equal_to = not_greater_than<Value>;
	template <typename Value> using greater_than_or_equal_to = not_less_than<Value>;

	// derivatives

	template <typename Value> using point = equal_to<Value>;
	template <typename Value> using hole = not_equal_to<Value>;
	template <typename Value> using ray = std::variant // (,) [,] (,] (,]
	<less_than<Value>, greater_than<Value>, not_less_than<Value>, not_greater_than<Value>>;

	template <typename Value> inline constexpr Value zero { };

	template <typename Value> struct non_zero : hole<Value>
	{
		non_zero() : hole<Value>(zero<Value>) { }
	};

	template <typename Value> struct positive : greater_than<Value>
	{
		positive() : greater_than<Value>(zero<Value>) { }
	};

	template <typename Value> struct non_negative : not_less_than<Value>
	{
		non_negative() : not_less_than<Value>(zero<Value>) { }
	};

	//
	// Intervals
	//

	template
	<
	 template <typename> typename LowerBound,
	 template <typename> typename UpperBound,
	 typename Value
	>
	struct pair
	: std::pair<LowerBound<Value>, UpperBound<Value>>
	, stl::not_copyable 
	{
		using value_type = Value;
		using lower_bound_type = LowerBound<value_type>;
		using upper_bound_type = UpperBound<value_type>;
		using base = std::pair<lower_bound_type, upper_bound_type>;

		static constexpr bool is_closed = lower_bound_type::is_closed and upper_bound_type::is_closed;
		static constexpr bool is_open = lower_bound_type::is_open and upper_bound_type::is_open;
		static constexpr bool is_convex = lower_bound_type::is_bounded_below and upper_bound_type::is_bounded_above;

		static_assert(lower_bound_type::is_bounded_below == upper_bound_type::is_bounded_above);
		static_assert(lower_bound_type::is_bounded_above == upper_bound_type::is_bounded_below);

		explicit pair(const value_type &lower, const value_type &upper) : base(lower, upper)
		{
			if (lower < upper) throw std::out_of_range("upper < lower");
		}

		bool operator==(const pair &that) const
		{
			return this->first == that.first and this->second == that.second;
		}

		bool contains(const value_type &x) const
		{
			if constexpr (is_convex)
			{
				return this->first(x) and this->second(x);
			}
			else
			{
				return this->first(x) or this->second(x);
			}
		}

		bool operator()(const value_type &x) const
		{
			return contains(x);
		}

		template
		<
		 template <typename> typename L,
		 template <typename> typename U,
		 typename V
		>
		bool is_subset_of(const pair<L, U, V> &that) const
		{
			return this->first.is_subset_of(that.first) and this->second.is_subset_of(that.second);
		}

		template
		<
		 template <typename> typename L,
		 template <typename> typename U,
		 typename V
		>
		bool operator<(const pair<L, U, V> &that) const
		{
			return is_subset_of(that);
		}
	};

	//
	// Alias Types
	//

	template <typename Value> using closed_interval = pair<not_less_than, not_greater_than, Value>;
	template <typename Value> using open_interval = pair<greater_than, less_than, Value>;
	template <typename Value> using left_open_interval = pair<not_less_than, less_than, Value>;
	template <typename Value> using right_open_interval = pair<greater_than, not_greater_than, Value>;

	// equivalents	

	template <typename Value> using left_closed_interval = right_open_interval<Value>;
	template <typename Value> using right_closed_interval = left_open_interval<Value>;

	// derivatives

	template <typename Value> using interval = std::variant // (,) [,] (,] [,)
	<open_interval<Value>, closed_interval<Value>, left_open_interval<Value>, right_open_interval<Value>>;
}

#endif // file

