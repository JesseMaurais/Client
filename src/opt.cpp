// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "opt.hpp"
#include "arg.hpp"
#include "fmt.hpp"
#include "dig.hpp"
#include "type.hpp"

namespace
{
	template
	<
		class Key, class Value, class Cast
	>
	auto cast(Key key, Value value, Cast cast)
	{
		const auto u = env::opt::get(key);
		return empty(u) ? value : cast(u);
	}

	auto cast(bool value)
	{
		return value ? "true" : "false";
	}

	template <class Key> bool cast(Key key, bool value)
	{
		const auto check = { cast(false), "0", "no", "off", "disable" };
		const auto u = env::opt::get(key);
		if (not u.empty())
		{
			const auto s = fmt::to_lower(u);
			for (const auto v : check)
			{
				if (s.starts_with(v))
				{
					return false;
				}
			}
			return true;
		}
		return value;
	}
}

namespace env::opt
{
	bool get(view key, bool value)
	{
		return cast(key, value);
	}

	bool set(view key, bool value)
	{
		fmt::view u = cast(value);
		return set(key, u);
	}

	bool get(pair key, bool value)
	{
		return cast(key, value);
	}

	bool set(pair key, bool value)
	{
		fmt::string::view u = cast(value);
		return set(key, u);
	}

	view get(view key, view value)
	{
		return got(key) ? get(key) : value;
	}

	view get(pair key, view value)
	{
		return got(key) ? get(key) : value;
	}

	long get(view key, long value, int base)
	{
		return cast(key, value, [base](auto value)
		{
			return fmt::to_long(value, base);
		});
	}

	bool set(view key, long value, int base)
	{
		return set(key, fmt::to_string(value, base));
	}

	long get(pair key, long value, int base)
	{
		return cast(key, value, [base](auto value)
		{
			return fmt::to_long(value, base);
		});
	}

	bool set(pair key, long value, int base)
	{
		return set(key, fmt::to_string(value, base));
	}

	float get(view key, float value)
	{
		return cast(key, value, [](auto value)
		{
			return fmt::to_float(value);
		});
	}

	bool set(view key, float value, int digits)
	{
		return set(key, fmt::to_string(value, digits));
	}

	float get(pair key, float value)
	{
		return cast(key, value, [](auto value)
		{
			return fmt::to_float(value);
		});
	}

	bool set(pair key, float value)
	{
		return set(key, fmt::to_string(value));
	}
}
