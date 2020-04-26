#include "opt.hpp"
#include "dig.hpp"

namespace
{
	template 
	<
		class Key, class Value, class Cast
	>
	auto cast(Key key, Value value, Cast cast)
	{
		auto const u = env::opt::get(key);
		return empty(u) ? value : cast(u);
	}

	auto cast(bool value)
	{
		return value ? "true" : "false";
	}

	template <class Key> bool cast(Key key, bool value)
	{
		auto const check = { cast(false), "0", "no", "off", "disable" };
		auto const u = env::opt::get(key);
		if (not empty(u))
		{
			auto const s = fmt::to_lower(u);
			for (auto const v : check)
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
	// Boolean

	bool get(word key, bool value)
	{
		return cast(key, value);
	}

	bool put(word key, bool value)
	{
		return put(key, cast(value));
	}

	bool get(pair key, bool value)
	{
		return cast(key, value);
	}

	bool put(pair key, bool value)
	{
		return put(key, cast(value));
	}

	// Integer

	word get(view key, word value, int base)
	{
		return cast(key, value, [base](auto value)
		{
			return fmt::to_llong(value, base);
		});
	}

	bool put(view key, word value, int base)
	{
		return put(key, fmt::to_string(value, base));
	}

	word get(pair key, word value, int base)
	{
		return cast(key, value, [base](auto value)
		{
			return fmt::to_llong(value, base);
		});
	}

	bool put(pair key, word value, int base)
	{
		return put(key, fmt::to_string(value, base));
	}

	// Pointed Decimal

	quad get(view key, quad value)
	{
		return cast(key, value, [](auto value)
		{
			return fmt::to_quad(value);
		});
	}

	bool put(view key, quad value, int digits)
	{
		return set(key, fmt::to_string(value, digits));
	}

	quad get(pair key, quad value)
	{
		return cast(key, value, [](auto value)
		{
			return fmt::to_quad(value);
		});
	}

	bool put(pair key, quad value)
	{
		return set(key, fmt::to_string(value));
	}
}
