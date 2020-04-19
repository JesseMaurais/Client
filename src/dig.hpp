#ifndef dig_hpp
#define dig_hpp "Digital Format"

#include <cmath>
#include <limits>
#include <type_traits>
#include "fmt.hpp"
#include "err.hpp"

namespace fmt
{
	template 
	<
		typename T
	> 
	constexpr auto zero = T { };

	template 
	<
		typename T
	>
	constexpr auto maximum = std::numeric_limits<T>::max();

	template 
	<
		typename T
	> 
	constexpr auto minimum = std::numeric_limits<T>::lowest();

	template 
	<
		typename T
	> 
	constexpr bool is_integer = std::is_integral<T>::value;

	template 
	<
		typename T
	> 
	constexpr bool is_signed = std::is_signed<T>::value;

	template 
	<
		typename T
	> 
	constexpr bool is_unsigned = std::is_unsigned<T>::value;

	template 
	<
		typename T
	> 
	using as_signed = typename std::make_signed<T>::type;

	template 
	<
		typename T
	> 
	using as_unsigned = typename std::make_unsigned<T>::type;

	template 
	<
		typename S
	> 
	auto to_unsigned(S s)
	{
		static_assert(is_integer<S>);
		static_assert(is_signed<S>);
		assert(zero<S> <= s);
		using T = as_unsigned<S>;
		return static_cast<T>(s);
	}

	template 
	<
		typename S
	>
	auto to_signed(S s)
	{
		static_assert(is_integer<S>);
		static_assert(is_unsigned<S>);
		using T = as_signed<S>;
		auto const t = static_cast<T>(s);
		assert(zero<T> <= t);
		return t;
	}

	template 
	<
		typename T, typename S
	> 
	auto to_sign_of(S s)
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

	template 
	<
		typename T, typename S
	> 
	auto to_narrow(S s)
	{
		std::less<S> less;
		static_assert(sizeof(T) < sizeof(S));
		static_assert(is_integer<S> and is_integer<T>);
		static_assert(is_signed<T> == is_signed<S>);
		static_assert(S{minimum<T>} <= zero<S>);
		static_assert(zero<S> <= S{maximum<T>});
		assert(not less(s, maximum<T>));
		assert(not less(minimum<T>, s));
		return static_cast<T>(s);
	}

	template 
	<
		typename T, typename S
	> 
	auto to(S s)
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

	template 
	<
		typename S
	> 
	auto to_size(S s)
	{
		return to<size_t>(s);
	}

	template 
	<
		typename S
	> 
	auto to_int(S s)
	{
		return to<int>(s);
	}

	template 
	<
		typename T
	> 
	bool fail(T t)
	{
		if constexpr (is_integer<T>)
		{
			return fail(static_cast<double>(t));
		}
		else
		{
			return std::isnan(t);
		}
	}

	string to_string(long value, int base);
	string to_string(long long value, int base);
	string to_string(unsigned long value, int base);
	string to_string(unsigned long long value, int base);
	string to_string(float value, int precision);
	string to_string(double value, int precision);
	string to_string(long double value, int precision);

	long to_long(string::view, int base = 10);
	long long to_llong(string::view, int base = 10);
	unsigned long to_ulong(string::view, int base = 10);
	unsigned long long to_ullong(string::view, int base = 10);
	float to_float(string::view);
	double to_double(string::view);
	long double to_quad(string::view);


}

namespace fmt::dig
{
	template
	<
		class Type
	>
	constexpr auto to(string::view line, int base = 10)
	{
		constexpr if (is_integer<Type>)
		{
			constexpr if (is_signed<Type>)
			{
				return to_narrow<Type>(to_llong(line, base));
			}
			else
			{
				return to_narrow<Type>(to_ullong(line, base));
			}
		}
		else
		{
			return to_narrow<Type>(to_quad(line));
		}
	}
}

#endif // file
