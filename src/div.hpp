#ifndef div_hpp
#define div_hpp "Numeric Division"

#include "dig.hpp"
#include <numeric>
#include <cinttypes>
#include <cstdlib>
#include <cfloat>
#include <cmath>

namespace fmt
{
	template <class N> using div = std::div_t;
	template <> using div<long> = std::ldiv_t;
	template <> using div<long long> = std::lldiv_t;
	template <> using div<std::intmax_t> = std::imaxdiv_t;

	template <class N, class F> to_div(F n)
	{
		static_assert(is_integer<N>);
		static_assert(not is_integer<F>);
		F quot;
		F const rem = std::modf(n, &quot);
		return div { to<N>(quot), to<N>(rem) };
	}
}

#endif // file
