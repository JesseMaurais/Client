#ifndef top_hpp
#define top_hpp

// Topology toolset based on a reading of James Munkres' book.

#include <utility>
#include <functional>
#include <type_traits>
#include "tool.hpp"

namespace top
{
	template <template <typename> typename Bound, typename Value>
	struct ray : stl::not_copyable
	{
		using value_type = Value;
		using boundary_type = Bound<value_type>;
		using base = ray;

	private:

		using less_than = std::less<value_type>;
		using greater_than = std::greater<value_type>;
		using not_less_than = std::greater_equal<value_type>;
		using not_greater_than = std::less_equal<value_type>;
		using less_or_equal_to = not_greater_than;
		using greater_or_equal_to = not_less_than;

		template <class C>
		static constexpr bool is_base_of = std::is_base_of_v<C, boundary_type>;

		template <class A, class B>
		static constexpr bool is_base_one_of = is_base_of<A> or is_base_of<B>;

	public:

		static constexpr bool is_open = is_base_one_of<less_than, greater_than>;
		static constexpr bool is_closed = is_base_one_of<not_less_than, not_greater_than>;
		static constexpr bool is_bounded_above = is_base_one_of<less_than, not_greater_than>;
		static constexpr bool is_bounded_below = is_base_one_of<greater_than, not_less_than>;

		const value_type &limit;
		const boundary_type compare;

		explicit ray(const value_type &ref)
		: limit(ref), compare()
		{ }

		bool operator==(const base &that) const
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
		bool is_subset_of(const ray<R, V> &that) const
		{
			static_assert(decltype(that)::is_bounded_above == is_bounded_below, "Argument cannot be a subset.");
			return that.contains(limit);
		}

		template <template <typename> typename R, typename V>
		bool operator<(const ray<R, V> &that) const
		{
			return is_subset_of(that);
		}
	};

	template <typename Value> using greater_than =  ray<std::greater, Value>;
	template <typename Value> using not_greater_than = ray<std::less_equal, Value>;
	template <typename Value> using less_than = ray<std::less, Value>;
	template <typename Value> using not_less_than = ray<std::greater_equal, Value>;
	template <typename Value> using less_or_equal_to = not_greater_than<Value>;
	template <typename Value> using greater_or_equal_to = not_less_than<Value>;

	template
	<
	 template <typename> typename LowerBound,
	 template <typename> typename UpperBound,
	 typename Value
	>
	struct interval
	: std::pair<LowerBound<Value>, UpperBound<Value>>
	, stl::not_copyable 
	{
		using value_type = Value;
		using lower_bound_type = LowerBound<value_type>;
		using upper_bound_type = UpperBound<value_type>;
		using base = std::pair<lower_bound_type, upper_bound_type>;

		static constexpr bool is_closed = base::first::is_closed and base::second::is_closed;
		static constexpr bool is_open = base::first::is_open and base::second::is_open;

		explicit interval(const value_type &lower, const value_type &upper)
		: base(lower, upper)
		{
			static_assert(lower_bound_type::is_bounded_below, "Lower boundary must be a lower bound.");
			static_assert(upper_bound_type::is_bounded_above, "Upper boundary must be a upper bound.");
		}

		bool contains(const value_type &x) const
		{
			return first(x) and second(x);
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
		bool is_subset_of(const interval<L, U, V> &that) const
		{
			return base::first.is_subset_of(that.first) and base::second.is_subset_of(that.second);
		}

		template
		<
		 template <typename> typename L,
		 template <typename> typename U,
		 typename V
		>
		constexpr bool operator()(const interval<L, U, V> &that) const
		{
			return is_subset_of(that);
		}
	};

	template <typename Value> using closed_interval = interval<not_less_than, not_greater_than, Value>;
	template <typename Value> using open_interval = interval<greater_than, less_than, Value>;
	template <typename Value> using left_open_interval = interval<not_less_than, less_than, Value>;
	template <typename Value> using right_open_interval = interval<greater_than, not_greater_than, Value>;
	template <typename Value> using left_closed_interval = right_open_interval<Value>;
	template <typename Value> using right_closed_interval = left_open_interval<Value>;
}

#endif // file

