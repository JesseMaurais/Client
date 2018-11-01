#ifndef top_hpp
#define top_hpp

// Topology toolset based on a reading of James Munkres' book.

#include <limits>
#include <utility>
#include <functional>
#include <type_traits>

namespace stl
{
	struct not_copyable
	{	
		not_copyable &operator=(const not_copyable &) = delete;
		not_copyable(const not_copyable &) = delete;
	};
}

namespace top
{
	template <template <typename> typename Relation, template Value>
	struct ray : stl::not_copyable
	{
		using limits = std::numeric_limits<Value>;
		using relation_type = Relation<Value>;
		using value_type = Value;
		using ray_type = ray;

		const value_type &limit;
		const relation_type compare;

		explicit ray(const value_type &ref)
		: limit(ref)
		{ 
			static_assert(limits::has_infinity, "Value numeric concept must have infinity");
		}

		constexpr bool contains(const value_type &x) const
		{
			return compare(x, limit);
		}
		constexpr bool operator()(const value_type &x) const
		{
			return contains(x);
		}

		bool is_closed() const
		{
			return contains(limit);
		}
		bool is_open() const
		{
			return not is_closed();
		}
		operator bool() const
		{
			return is_closed();
		}

		constexpr bool is_bounded_above() const
		{
			return not contains(limits::infinity);
		}
		constexpr bool is_bounded_below() const
		{
			return contains(limits::infinity);
		}

		bool operator==(const ray_type &a) const
		{
			return limit == a.limit;
		}

		template <template <typename> typename R, typename V>
		bool subset_of(const ray<R, V> &a) const
		{
			return a(limit) and not contains(a.limit);
		}
		template <template <typename> typename R, typename V>
		bool operator<(const ray<R, V> &a) const
		{
			return subset_of(a);
		}
	};

	template <typename Value> using greater_than =  ray<std::greater, Value>;
	template <typename Value> using not_greater_than = ray<std::less_equal, Value>;
	template <typename Value> using less_than = ray<std::less, Value>;
	template <typename Value> using not_less_than = ray<std::greater_equal, Value>;
	// equivalent expressions
	template <typename Value> using less_or_equal_to = std::not_greater_than<Value>;
	template <typename Value> using greater_or_equal_to = std::not_less_than<Value>;

	template <template <typename> typename Relation, typename Value> using ray_complement = void;
	template <typename Value> using ray_complement<greater_than, Value> = not_greater_than<Value>;
	template <typename Value> using ray_complement<not_greater_than, Value> = greater_than<Value>;
	template <typename Value> using ray_complement<less_than, Value> = not_less_than<Value>;

	template
	<
	 template <typename> typename RightBoundary,
	 template <typename> typename LeftBoundary,
	 typename Value
	>
	struct interval
	: std::pair<RightBoundary<Value>, LeftBoundary<Value>>
	, stl::not_copyable 
	{
		using right_boundary_type = pair::first_type;
		using left_boundary_type = pair::second_type;
		using value_type = Value;

		explicit interval(const value_type& right, const value_type& left)
		: pair(std::min(right, left), std::max(left, right)
		{
			static_assert
			(
			 right_boundary_type::bounded_below() == left_boundary_type::bounded_above(),
			 "Not an interval. One ray will contain the other."
			);
		}

		constexpr bool is_closed() const
		{
			return first.is_closed() and second.is_closed();
		}
		constexpr bool is_open() const
		{
			return first.is_open() and second.is_open();
		}
		constexpr operator bool() const
		{
			return is_closed();
		}

		constexpr bool is_convex() const
		{
			return first(second.value) and second(first.value);
		}

		constexpr bool contains(const value_type &x) const
		{
			return first(x) and second(x);
		}
		constexpr bool operator()const value_type &x) const
		{
			return contains(x);
		}

		template
		<
		 template <typename> typename R,
		 template <typename> typename L,
		 typename V
		>
		constexpr bool contains(const interval<R, L, V> &i) const
		{
			return i.first(first.value) and i.second(second.value);
		}
		template
		<
		 template <typename> typename R,
		 template <typename> typename L,
		 typename V
		>
		constexpr bool operator()(const interval<R, L, V> &i) const
		{
			return contains(i);
		}
	};

	template <typename Value> using closed_interval = interval<not_less_than, not_greater_than, Value>;
	template <typename Value> using open_interval = interval<greater_than, less_than, Value>;
	typename <typename Value> using left_open_interval = interval<not_less_than, less_than, Value>;
	typename <typename Value> using right_open_interval = interval<greater_than, not_greater_than, Value>;
	// equivalent expressions
	template <typename Value> using left_closed_interval = right_open_interval<Value>;
i	template <typename Value> using right_closed_interval = left_open_interval<Value>;
}

#endif // file

