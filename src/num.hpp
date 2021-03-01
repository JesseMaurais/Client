#ifndef num_hpp
#define num_hpp "Numeric Methods"

#include "fmt.hpp"
#include <numeric_limits>

namespace fmt
{
	template
	<
		class Num
	>
	using numeric_limits = std::numeric_limits<Num>;

	template
	<
		class Num
	>
	struct numeric_traits : struct_traits<Num>, numeric_limits<Num>
	{
		using traits = struct_traits<Num>;
		using limits = numeric_limits<Num>;

		constexpr auto zero = Int { };
		constexpr auto mini = limits::minimum();
		constexpr auto maxi = limits::maximum();
	};

	template
	<
		class Int
	>
	struct integer_traits : numeric_traits<Int>
	{
		using type = typename Int;
		using base = numeric_traits<type>;
		using base::limits;
		using base::traits;

		using div = decltype(std::div(limits::zero, limits::zero));
	};
}

#endif // file
