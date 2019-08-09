#include "int.hpp"
#include <cstdlib>
#include <cmath>

namespace
{
	template <typename T, typename C>
	T to_fp(fmt::string_view u, T nan, C* to)
	{
		auto begin = data(u);
		char* end;
		auto const value = to(begin, &end);
		if (begin == end)
		{
			sys::err(here, u);
			return nan;
		}
		return value;
	}

	template <typename T, typename C>
	T to_base(fmt::string_view u, T def, int base, C* to)
	{
		auto begin = data(u);
		char* end;
		auto const value = to(begin, &end, base);
		if (begin == end)
		{
			sys::err(here, u, "in base", base);
			return def;
		}
		return value;
	}

	constexpr auto arg = "";
	template <typename T> T inan()
	{
		auto const nan = std::nan(arg);
		return static_cast<T>(nan);
	}
}

namespace fmt
{
	long to_long(string_view u, int base)
	{
		auto const nan = inan<long>();
		return to_base(u, nan, base, std::strtol);
	}

	long long to_llong(string_view u, int base)
	{
		auto const nan = inan<long long>();
		return to_base(u, nan, base, std::strtoll);
	}

	unsigned long to_ulong(string_view u, int base)
	{
		auto const nan = inan<unsigned long>();
		return to_base(u, nan, base, std::strtoul);
	}

	unsigned long long to_ullong(string_view u, int base)
	{
		auto const nan = inan<unsigned long long>();
		return to_base(u, nan, base, std::strtoull);
	}

	float to_float(string_view u)
	{
		auto const nan = std::nanf(arg);
		return to_fp(u, nan, std::strtof);
	}

	double to_double(string_view u)
	{
		auto const nan = std::nan(arg);
		return to_fp(u, nan, std::strtod);
	}

	long double to_ldouble(string_view u)
	{
		auto const nan = std::nanl(arg);
		return to_fp(u, nan, std::strtold);
	}
}

