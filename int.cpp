#include "int.hpp"
#include "fmt.hpp"
#include <charconv>
#include <cstdlib>
#include <cmath>

namespace
{
	template <typename T, typename C>
	T to_fp(fmt::string_view u, T nan, C* to)
	{
		char* it;
		auto ptr = data(u);
		auto const value = to(ptr, &it);
		if (ptr == it)
		{
			sys::err(here, u);
			return nan;
		}
		return value;
	}

	template <typename T>
	T to_base(fmt::string_view u, int base)
	{
		auto value = static_cast<T>(std::nan(""));
		auto begin = data(u);
		auto end = begin + size(u);
		auto code = std::from_chars(begin, end, value, base);
		if (sys::noerr != code.ec)
		{
			sys::warn(here, code.ec);
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
			if (sys::noerr != code.ec)
			{
				if (std::errc::value_too_large == code.ec)
				{
					s.resize(2*size(s), '\0');
					continue;
				}
				sys::warn(here, code.ec);
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
		return to_base<long>(u, base);
	}

	long long to_llong(string_view u, int base)
	{
		return to_base<long long>(u, base);
	}

	unsigned long to_ulong(string_view u, int base)
	{
		return to_base<unsigned long>(u, base);
	}

	unsigned long long to_ullong(string_view u, int base)
	{
		return to_base<unsigned long long>(u, base);
	}

	float to_float(string_view u)
	{
		auto const nan = std::nanf("");
		return to_fp(u, nan, std::strtof);
	}

	double to_double(string_view u)
	{
		auto const nan = std::nan("");
		return to_fp(u, nan, std::strtod);
	}

	long double to_quad(string_view u)
	{
		auto const nan = std::nanl("");
		return to_fp(u, nan, std::strtold);
	}
}

