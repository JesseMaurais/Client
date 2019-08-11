#ifndef int_hpp
#define int_hpp

#include <cmath>
#include <limits>
#include <type_traits>
#include "err.hpp"

namespace fmt
{
	template <typename T> constexpr T zero { };
	template <typename T> constexpr T maximum = std::numeric_limits<T>::max();
	template <typename T> constexpr T minimum = std::numeric_limits<T>::lowest();

	template <typename T> using as_signed = typename std::make_signed<T>::type;
	template <typename T> using as_unsigned = typename std::make_unsigned<T>::type;

	template <typename T> constexpr bool is_integral = std::is_integral<T>::value;
	template <typename T> constexpr bool is_signed = std::is_signed<T>::value;
	template <typename T> constexpr bool is_unsigned = std::is_unsigned<T>::value;

	template <typename S> inline auto to_unsigned(S s)
	{
		static_assert(is_integral<S>);
		static_assert(is_signed<S>);
		assert(zero<S> <= s);
		using T = as_unsigned<S>;
		return static_cast<T>(s);
	}

	template <typename S> inline auto to_signed(S s)
	{
		static_assert(is_integral<S>);
		static_assert(is_unsigned<S>);
		using T = as_signed<S>;
		auto const t = static_cast<T>(s);
		assert(zero<T> <= t);
		return t;
	}

	template <typename T, typename S> inline auto to_sign_of(S s)
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
	
	template <typename T, typename S> inline T to_narrow(S s)
	{
		static_assert(sizeof(T) < sizeof(S));
		static_assert(is_integral<S> and is_integral<T>);
		static_assert(is_signed<T> == is_signed<S>);
		static_assert(S{minimum<T>} <= zero<S>);
		static_assert(zero<S> <= S{maximum<T>});
		assert(s <= S{maximum<T>});
		assert(S{minimum<T>} <= s);
		return static_cast<T>(s);
	}

	template <typename T, typename S> inline T to(S s)
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

	template <typename S> inline auto to_size(S s)
	{
		return to<size_t>(s);
	}

	template <typename S> inline auto to_int(S s)
	{
		return to<int>(s);
	}

	long to_long(string_view, int base = 10);
	long to_long(wstring_view, int base = 10);
	long long to_llong(string_view, int base = 10);
	long long to_llong(wstring_view, int base = 10);
	unsigned long to_ulong(string_view, int base = 10);
	unsigned long to_ulong(wstring_view, int base = 10);
	unsigned long long to_ullong(string_view, int base = 10);
	unsigned long long to_ullong(wstring_view, int base = 10);

	float to_float(string_view);
	float to_float(wstring_view);
	double to_double(string_view);
	double to_double(wstring_view);
	long double to_quad(string_view);
	long double to_quad(wstring_view);

	template <typename T>
	inline bool fail(T t)
	{
		return std::isnan(t);
	}
}

#endif // file
