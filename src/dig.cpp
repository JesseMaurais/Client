// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "err.hpp"
#include "dig.hpp"
#include <iomanip>
#include <charconv>

namespace
{
	const std::errc noerr { };

	template <class T, class C> T to_fp(fmt::view u, T nan, C* cast)
	{
		char *it;
		auto ptr = u.data();
		const auto value = cast(ptr, &it);
		if (ptr == it)
		{
			return nan;
		}
		return value;
	}

	template <class T> T to_base(fmt::view u, int base)
	{
		auto begin = u.data();
		auto end = begin + u.size();
		auto value = static_cast<T>(std::nan(""));
		auto code = std::from_chars(begin, end, value, base);
		if (noerr != code.ec)
		{
			auto const error = std::make_error_code(code.ec);
			auto const message = error.message();
			fmt::out() << message;
		}
		return value;
	}

	template <class T> fmt::string from_base(T value, int base)
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
				const auto error = std::make_error_code(code.ec);
				const auto message = error.message();
				fmt::out() << message;
			}
			s.resize(code.ptr - begin);
		}
		while (0);
		s.shrink_to_fit();
		return s;
	}

	template <class T> fmt::string from_fp(T value, int precision)
	{
		/* we somehow still have no compiler support
		fmt::string s(20, '\0');
		do
		{
			auto begin = s.data();
			auto end = begin + s.size();
			constexpr auto format = std::chars_format::general;
			auto code = std::to_chars(begin, end, value, format, precision);
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
		/*/
		std::stringstream ss;
		ss << std::fixed << std::setprecision(precision) << value;
		return ss.str();
		//*/
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

	long to_long(view u, int base)
	{
		return to_base<long>(u, base);
	}

	long long to_llong(view u, int base)
	{
		return to_base<long long>(u, base);
	}

	unsigned long to_ulong(view u, int base)
	{
		return to_base<unsigned long>(u, base);
	}

	unsigned long long to_ullong(view u, int base)
	{
		return to_base<unsigned long long>(u, base);
	}

	float to_float(view u)
	{
		auto const nan = std::nanf("");
		return to_fp(u, nan, std::strtof);
	}

	double to_double(view u)
	{
		auto const nan = std::nan("");
		return to_fp(u, nan, std::strtod);
	}

	long double to_quad(view u)
	{
		auto const nan = std::nanl("");
		return to_fp(u, nan, std::strtold);
	}
}

#ifdef TEST
TEST(dig)
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
