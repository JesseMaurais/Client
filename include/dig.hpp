#ifndef dig_hpp
#define dig_hpp "Digital Format"

#include "fmt.hpp"
#include <limits>
#include <cmath>
#include <type_traits>

namespace fmt
{
	template <class N> constexpr auto zero = N { };
	template <class N> constexpr auto greatest = std::numeric_limits<N>::max();
	template <class N> constexpr auto least = std::numeric_limits<N>::lowest();

	template <class N> constexpr bool is_integer = std::is_integral<N>::value;
	template <class N> constexpr bool is_signed = std::is_signed<N>::value;
	template <class N> constexpr bool is_unsigned = std::is_unsigned<N>::value;

	template <class N> using as_signed = typename std::make_signed<N>::type;
	template <class N> using as_unsigned = typename std::make_unsigned<N>::type;

	template <class N> auto to_unsigned(N n)
	{
		static_assert(is_integer<N>);
		static_assert(is_signed<N>);
		using M = as_unsigned<N>;
		#ifdef assert
		assert(zero<N> <= n);
		#endif
		return static_cast<M>(n);
	}

	template <class N> auto to_signed(N n)
	{
		static_assert(is_integer<N>);
		static_assert(is_unsigned<N>);
		using M = as_signed<N>;
		#ifdef assert
		assert(n <= static_cast<N>(greatest<M>));
		#endif
		return static_cast<M>(n);
	}

	template <class N, class M> auto to_narrow(M n)
	{
		static_assert(sizeof(N) < sizeof(M));
		static_assert(is_integer<N> and is_integer<M>);
		static_assert(is_signed<N> == is_signed<M>);
		#ifdef assert
		assert(n <= static_cast<M>(greatest<N>));
		assert(static_cast<M>(least<N>) <= n);
		#endif
		return static_cast<N>(n);
	}

	template <class N, class M> auto to_wide(M n)
	{
		static_assert(sizeof(N) > sizeof(M));
		static_assert(is_integer<N> and is_integer<M>);
		static_assert(is_signed<N> == is_signed<M>);
		return static_cast<N>(n);
	}

	template <class N, class M> auto to_sign_of(M n)
	{
		if constexpr (is_signed<M> and is_unsigned<N>)
		{
			return to_unsigned(n);
		}
		else
		if constexpr (is_unsigned<M> and is_signed<N>)
		{
			return to_signed(n);
		}
		else
		{
			return n;
		}
	}

	template <class N, class M> auto to(M n)
	{
		if constexpr (sizeof(N) < sizeof(M))
		{
			return to_narrow<N>(to_sign_of<N>(n));
		}
		else
		{
			return to_sign_of<N>(n);
		}
	}

	template <class N> inline auto to_size(N n)
	{
		return to<size_t>(n);
	}

	template <class N> inline auto to_int(N n)
	{
		return to<int>(n);
	}

	template <class C, class N> bool in_range(const C& c, N n)
	{
		auto const sz = std::size(c);
		if constexpr (is_signed<N>)
		{
			return -1 < n and to_unsigned(n) < sz;
		}
		else
		{
			return n < sz;
		}
	}

	string to_string(long value, int base);
	string to_string(long long value, int base);
	string to_string(unsigned long value, int base);
	string to_string(unsigned long long value, int base);
	string to_string(float value, int precision);
	string to_string(double value, int precision);
	string to_string(long double value, int precision);

	long to_long(view, int base = 10);
	long long to_llong(view, int base = 10);
	unsigned long to_ulong(view, int base = 10);
	unsigned long long to_ullong(view, int base = 10);
	float to_float(view);
	double to_double(view);
	long double to_quad(view);
}

#endif // file
