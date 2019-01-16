#ifndef int_hpp
#define int_hpp

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
		assert(T(0) <= s);
		return static_cast<to_type>(s);
	}

	template <typename T>
	inline auto to_signed(T const& u)
	{
		using to_type = typename std::make_signed<T>::type;
		static_assert(std::is_unsigned<T>::value);
		static_assert(std::is_integral<T>::value);
		auto const s = static_cast<to_type>(u);
		assert(T(0) <= s);
		return s;
	}

	template <typename T>
	inline std::size_t to_size(T const& t)
	{
		if constexpr (std::is_signed<T>::value)
		{
			auto const u = to_unsigned(t);
			return static_cast<std::size_t>(u);
		}
		else
		{
			return static_cast<std::size_t>(t);
		}
	}
}

#endif // file
