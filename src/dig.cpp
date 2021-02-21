// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dig.hpp"
#include "fmt.hpp"
#include <sstream>
#include <charconv>
#include <system_error>
#include <iomanip>
#include <cstdlib>
#include <cmath>

namespace
{
	std::errc const noerr { };

	template <typename T, typename C>
	T to_fp(fmt::string::view u, T nan, C* cast)
	{
		char *it;
		auto ptr = u.data();
		auto const value = cast(ptr, &it);
		if (ptr == it)
		{
			sys::err(here, u);
			return nan;
		}
		return value;
	}

	template <typename T>
	T to_base(fmt::string::view u, int base)
	{
		auto begin = u.data();
		auto end = begin + u.size();
		auto value = static_cast<T>(std::nan(""));
		auto code = std::from_chars(begin, end, value, base);
		if (noerr != code.ec)
		{
			auto const error = std::make_error_code(code.ec);
			auto const message = error.message();
			sys::warn(here, message);
		}
		return value;
	}

	template <typename T>
	fmt::string from_base(T value, int base)
	{
		fmt::string s(20, '\0');
		do
		{
			auto begin = data(s);
			auto end = begin + size(s);
			auto code = std::to_chars(begin, end, value, base);
			if (noerr != code.ec)
			{
				if (std::errc::value_too_large == code.ec)
				{
					s.resize(2*s.size(), '\0');
					continue;
				}
				auto const error = std::make_error_code(code.ec);
				auto const message = error.message();
				sys::warn(here, message);
			}
			s.resize(code.ptr - begin);
		}
		while (0);
		s.shrink_to_fit();
		return s;
	}

	template <typename T>
	fmt::string from_fp(T value, int precision)
	{
		/* we somehow still have no compiler support
		fmt::string s(20, '\0');
		do
		{
			auto begin = s.data();
			auto end = begin + s.size();
			auto code = std::to_chars(begin, end, value, 0, precision);
			if (std::errc::value_too_large == code.ec)
			{
				s.resize(2*s.size(), '\0');
				continue;
			}
			s.resize(code.ptr - begin);
		}
		while (0);
		s.shrink_to_fit();
		return s;
		*/
		std::stringstream ss;
		ss << std::fixed << std::setprecision(precision) << value;
		return ss.str();
	}
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

	long to_long(string::view u, int base)
	{
		return to_base<long>(u, base);
	}

	long long to_llong(string::view u, int base)
	{
		return to_base<long long>(u, base);
	}

	unsigned long to_ulong(string::view u, int base)
	{
		return to_base<unsigned long>(u, base);
	}

	unsigned long long to_ullong(string::view u, int base)
	{
		return to_base<unsigned long long>(u, base);
	}

	float to_float(string::view u)
	{
		auto const nan = std::nanf("");
		return to_fp(u, nan, std::strtof);
	}

	double to_double(string::view u)
	{
		auto const nan = std::nan("");
		return to_fp(u, nan, std::strtod);
	}

	long double to_quad(string::view u)
	{
		auto const nan = std::nanl("");
		return to_fp(u, nan, std::strtold);
	}
}

#ifdef test
test(dig)
{
	assert('*' == fmt::to_narrow<char>(42));
	assert(42u == fmt::to_unsigned(42));
	assert(42 == fmt::to_signed(42u));
	assert(42 == fmt::to_long("42"));
	assert(42 == fmt::to_long("52", 8));
	assert(42 == fmt::to_long("2A", 16));
	assert(0.42f == fmt::to_float("0.42f"));
	assert(std::isnan(fmt::to_float("nan")));
	assert(std::isinf(fmt::to_double("inf")));
	// catch
	//except(256 == (int) fmt::to_narrow<char>(256));
	//except(-1 == (signed) fmt::to_unsigned(-1));
	//except(0L == fmt::to_long("$"));
}
#endif
