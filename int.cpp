#include "int.hpp"
#include <cstdlib>
#include <cwchar>
#include <cmath>

namespace
{
	template <typename T, typename V, typename C>
	T to_fp(V u, T nan, C* to)
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

	template <typename T> T inan()
	{
		auto const nan = std::nan("");
		return static_cast<T>(nan);
	}

	template <typename T, typename V, typename C>
	T to_base(View u, int base, C* to)
	{
		auto begin = data(u);
		char* end;
		auto const value = to(begin, &end, base);
		if (begin == end)
		{
			sys::err(here, u, "in base", base);
			return inan<T>();
		}
		return value;
	}
}

namespace fmt
{
	long to_long(string_view u, int base)
	{
		return to_base(u, base, std::strtol);
	}

	long to_long(wstring_view u, int base)
	{
		return to_base(u, base, std::wcstol);
	}

	long long to_llong(string_view u, int base)
	{
		return to_base(u, base, std::strtoll);
	}

	long long to_llong(wstring_view u, int base)
	{
		return to_base(u, base, std::wcstoll);
	}

	unsigned long to_ulong(string_view u, int base)
	{
		return to_base(u, base, std::strtoul);
	}

	unsigned long to_ulong(wstring_view u, int base)
	{
		return to_base(u, base, std::wcstoul);
	}

	unsigned long long to_ullong(string_view u, int base)
	{
		return to_base(u, base, std::strtoull);
	}

	unsigned long long to_ullong(wstring_view u, int base)
	{
		return to_base(u, base, std::wcstoull);
	}

	float to_float(string_view u)
	{
		auto const nan = std::nanf("");
		return to_fp(u, nan, std::strtof);
	}

	float to_float(wstring_view w)
	{
		auto const nan = std::nanf("");
		return to_fp(w, nan, std::wcstof);
	}

	double to_double(string_view u)
	{
		auto const nan = std::nan("");
		return to_fp(u, nan, std::strtod);
	}

	double to_double(wstring_view w)
	{
		auto const nan = std::nan("");
		return to_fp(w, nan, std::wcstod);
	}

	long double to_quad(string_view u)
	{
		auto const nan = std::nanl("");
		return to_fp(u, nan, std::strtold);
	}

	long double to_quad(wstring_view u)
	{
		auto const man = std::nanl("");
		return to_fp(u, nan, std::wcstold);
	}
}

