#ifndef range_hpp
#define range_hpp

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

	template <template <typename> typename Relation, template Value>
	struct comparison : not_copyable
	{
		using relation_type = Relation<Value>;
		using value_type = Value;

		const value_type &value;
		const relation_type relation;

		explicit comparison(const value_type &ref)
		: value(ref)
		{ }

		constexpr bool operator()(const value_type &x) const
		{
			return relation(value, x);
		}
	};

	template <typename Value> using equal_to = comparison<std::equal_to, Value>;
	template <typename Value> using not_equal_to = comparison<std::not_equal_to, Value>;
	template <typename Value> using greater_than =  comparison<std::greater, Value>;
	template <typename Value> using not_greater_than = comparison<std::less_equal, Value>;
	template <typename Value> using less_than = comparison<std::less, Value>;
	template <typename Value> using not_less_than = comparison<std::greater_equal, Value>;
	// equivalent expressions
	template <typename value> using equals = equal_to<Value>;
	template <typename Value> using less_or_equal_to = std::not_greater_than<Value>;
	template <typename Value> using greater_or_equal_to = std::not_less_than<Value>;


	template <typename Value> using paired = std::pair<Value, Value>;

	template
	<
	 template <typename> typename RightBoundary,
	 template <typename> typename LeftBoundary,
	 typename Value
	>
	struct interval
	: std::pair<RightBoundary<Value>, LeftBoundary<Value>>
	, not_copyable 
	{
		using right_boundary_type = pair::first_type;
		using left_boundary_type = pair::second_type;
		using value_type = Value;

		explicit interval(const value_type& right, const value_type& left)
		: pair(std::min(right, left), std::max(left, right)
		{ }

		constexpr bool operator()(const value_type &x) const
		{
			return first(x) and second(x);
		}

		constexpr bool operator<(const interval &i) const
		{
			return i.first(first.value) and i.second(second.value);
		}

	private:

		template <typename A, typename B>
		constexpr bool is_same()
		{
			return std::is_same<A, B>::value;
		}

		template <typename A, typename B>
		constexpr bool is_different()
		{
			return not is_same<A, B>();
		}

		static_assert
		(
		 is_different<right_boundary_type, greater_than>(),
		 "right boundary must face left"
		);

		static_assert
		(
		 is_different<right_boundary_type, not_less_than>(),
		 "right boundary must face left"
		);

		static_assert
		(
		 is_different<left_boundary_type, less_than>(),
		 "left boundary must face right"
		);

		static_assert
		(
		 is_different<left_boundary_type, not_greater_than>(),
		 "left boundary must face right"
		);
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

