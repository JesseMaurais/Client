#include "int.hpp"
#include "fmt.hpp"
#include <charconv>
#include <cstdlib>
#include <cwchar>
#include <cmath>

namespace
{
	template <typename T, typename V, typename C>
	T to_fp(V u, T nan, C* to)
	{
		auto ptr = data(u);
		typename V::value_type* it;
		auto const value = to(ptr, &it);
		if (ptr == it)
		{
			auto const s = fmt::to_string(u);
			sys::err(here, s);
			return nan;
		}
		return value;
	}

	template <typename T>
	const auto inan = static_cast<T>(std::nan(""));

	template <typename T, typename V, typename C>
	T to_base(V u, int base, C* to)
	{
		auto ptr = data(u);
		typename V::value_type* it;
		auto const value = to(ptr, &it, base);
		if (ptr == it)
		{
			auto const s = fmt::to_string(u);
			sys::err(here, s, "in base", base);
			return inan<T>;
		}
		return value;
	}

	template <typename T>
	fmt::string from_base(T value, int base)
	{
		fmt::string s(20, '\0');
		std::to_chars_result code;
		do
		{
			auto begin = data(s);
			auto end = begin + size(s);
			code = std::to_chars(begin, end, value, base);
			if (std::errc::value_too_large == code.ec)
			{
				s.resize(2*size(s), '\0');
				continue;
			}
			s.resize(code.ptr - begin);
		}
		while (false);
		return s;
	}

	/* Not supported
	template <typename T>
	fmt::string from_fp(T value, int precision)
	{
		fmt::string s(20, '\0');
		std::to_chars_result code;
		do
		{
			auto begin = data(s);
			auto end = begin + size(s);
			code = std::to_chars(begin, end, value, 0, precision);
			if (std::errc::value_too_large == code.ec)
			{
				s.resize(2*size(s), '\0');
				continue;
			}
			s.resize(code.ptr - begin);
		}
		while (false);
		return s;	
	}
	*/
}

namespace fmt
{
	string to_string(long value, int base)
	{
		return from_base<long>(value, base);
	}

	string to_string(long long value, int base)
	{
		return from_base<long long>(value, base);
	}

	string to_string(unsigned long value, int base)
	{
		return from_base<unsigned long>(value, base);
	}

	string to_string(unsigned long long value, int base)
	{
		return from_base<unsigned long long>(value, base);
	}

	/*
	string to_string(float value, int precision)
	{
		return from_fp<float>(value, precision);
	}

	string to_string(double value, int precision)
	{
		return from_fp<double>(value, precision);
	}

	string to_string(long double value, int precision)
	{
		return from_fp<long double>(value, precision);
	}
	*/

	long to_long(string_view u, int base)
	{
		return to_base<long>(u, base, std::strtol);
	}

	long to_long(wstring_view u, int base)
	{
		return to_base<long>(u, base, std::wcstol);
	}

	long long to_llong(string_view u, int base)
	{
		return to_base<long long>(u, base, std::strtoll);
	}

	long long to_llong(wstring_view u, int base)
	{
		return to_base<long long>(u, base, std::wcstoll);
	}

	unsigned long to_ulong(string_view u, int base)
	{
		return to_base<unsigned long>(u, base, std::strtoul);
	}

	unsigned long to_ulong(wstring_view u, int base)
	{
		return to_base<unsigned long>(u, base, std::wcstoul);
	}

	unsigned long long to_ullong(string_view u, int base)
	{
		return to_base<unsigned long long>(u, base, std::strtoull);
	}

	unsigned long long to_ullong(wstring_view u, int base)
	{
		return to_base<unsigned long long>(u, base, std::wcstoull);
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
		auto const nan = std::nanl("");
		return to_fp(u, nan, std::wcstold);
	}
}

