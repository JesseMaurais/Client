#ifndef dig_hpp
#define dig_hpp

#include <cmath>
#include <limits>
#include <type_traits>
#include "str.hpp"
#include "err.hpp"

namespace fmt
{
	template <typename T> constexpr T zero { };
	template <typename T> constexpr T maximum = std::numeric_limits<T>::max();
	template <typename T> constexpr T minimum = std::numeric_limits<T>::lowest();

	template <typename T> using as_signed = typename std::make_signed<T>::type;
	template <typename T> using as_unsigned = typename std::make_unsigned<T>::type;

	template <typename T> constexpr bool is_integer = std::is_integral<T>::value;
	template <typename T> constexpr bool is_signed = std::is_signed<T>::value;
	template <typename T> constexpr bool is_unsigned = std::is_unsigned<T>::value;

	template <typename S> auto to_unsigned(S s)
	{
		static_assert(is_integer<S>);
		static_assert(is_signed<S>);
		assert(zero<S> <= s);
		using T = as_unsigned<S>;
		return static_cast<T>(s);
	}

	template <typename S> auto to_signed(S s)
	{
		static_assert(is_integer<S>);
		static_assert(is_unsigned<S>);
		using T = as_signed<S>;
		auto const t = static_cast<T>(s);
		assert(zero<T> <= t);
		return t;
	}

	template <typename T, typename S> auto to_sign_of(S s)
	{
		if constexpr (is_signed<S> and is_unsigned<T>)
		{
			return to_unsigned(s);
		}
		else
		if constexpr (is_unsigned<S> and is_signed<T>)
		{
			return to_signed(s);
		}
		else
		{
			return s;
		}
	}

	template <typename T, typename S> auto to_narrow(S s)
	{
		static_assert(sizeof(T) < sizeof(S));
		static_assert(is_integer<S> and is_integer<T>);
		static_assert(is_signed<T> == is_signed<S>);
		static_assert(S{minimum<T>} <= zero<S>);
		static_assert(zero<S> <= S{maximum<T>});
		assert(s <= S{maximum<T>});
		assert(S{minimum<T>} <= s);
		return static_cast<T>(s);
	}

	template <typename T, typename S> auto to(S s)
	{
		if constexpr (sizeof(T) < sizeof(S))
		{
			return to_narrow<T>(to_sign_of<T>(s));
		}
		else
		{
			return to_sign_of<T>(s);
		}
	}

	template <typename S> auto to_size(S s)
	{
		return to<size_t>(s);
	}

	template <typename S> auto to_int(S s)
	{
		return to<int>(s);
	}

	string to_string(long value, int base);
	string to_string(long long value, int base);
	string to_string(unsigned long value, int base);
	string to_string(unsigned long long value, int base);

	long to_long(view, int base = 10);
	long long to_llong(view, int base = 10);
	unsigned long to_ulong(view, int base = 10);
	unsigned long long to_ullong(view, int base = 10);

	float to_float(view);
	double to_double(view);
	long double to_quad(view);

	template <typename T> bool fail(T t)
	{
		if constexpr (is_integer<T>)
		{
			auto const d = static_cast<double>(t);
			return std::isnan(d);
		}
		else
		{
			return std::isnan(t);
		}
	}
}

#endif // file
