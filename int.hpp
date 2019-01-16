#ifndef int_hpp
#define int_hpp

#include <cmath>
#include <cstddef>
#include <type_traits>
#include "dbg.hpp"

namespace
{
	template <typename T>
	inline auto to_unsigned(T const& s)
	{
		using to_type = typename std::make_unsigned<T>::type;
		static_assert(std::is_signed<T>::value);
		static_assert(std::is_integral<T>::value);
		assert(not std::signbit(s));
		return static_cast<to_type>(s);
	}

	template <typename T>
	inline auto to_signed(T const& u)
	{
		using to_type = typename std::make_signed<T>::type;
		static_assert(std::is_unsigned<T>::value);
		static_assert(std::is_integral<T>::value);
		auto const s = static_cast<to_type>(u);
		assert(not std::signbit(s));
		return s;
	}

	template <typename T>
	inline auto to_size(T const& t)
	{
		return static_cast<std::size_t>(t);
	}
}

#endif // file
