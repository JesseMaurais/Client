#ifndef int_hpp
#define int_hpp

#include <cstddef>
#include <type_traits>
#include <limits>

namespace
{
	template <typename T> constexpr T zero { };
	template <typename T> constexpr bool is_integral = std::is_integral<T>::value;
	template <typename T> constexpr bool is_signed = std::is_signed<T>::value;
	template <typename T> constexpr bool is_unsigned = std::is_unsigned<T>::value;
	template <typename T> using as_signed = typename std::make_signed<T>::type;
	template <typename T> using as_unsigned = typename std::make_unsigned<T>::type;

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
		using L = std::numeric_limits<T>;
		constexpr auto min = L::min();
		constexpr auto max = L::max();
		assert(s <= S{max});
		assert(S{min} <= s);
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
		return to<std::size_t>(s);
	}

	template <typename S> inline auto to_int(S s)
	{
		return to<int>(s);
	}
}

#endif // file
