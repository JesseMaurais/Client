#ifndef tmp_hpp
#define tmp_hpp

#include <functional>

template <typename... T> struct formula : std::function<bool(T...)>
{
	using base = std::function<bool(T...)>;
	using base::base;

	formula operator and(base const& that) const
	{
		return [&](T... x)
		{
			return (*this)(x...) and that(x...);
		};
	}

	formula operator or(base const& that) const
	{
		return [&](T... x)
		{
			return (*this)(x...) or that(x...);
		};
	}

	formula operator xor(base const& that) const
	{
		return [&](T... x)
		{
			return (*this)(x...) xor that(x...);
		};
	}

	formula operator not() const
	{
		return [&](T... x)
		{
			return not (*this)(x...);
		};
	}
};

template <typename T> 
using predicate = formula<T>;
template <typename T, typename S> 
using relation = formula<T, S>;
template <bool P, typename... Q> struct property : formula<Q...>
{
	using basic = formula<Q...>;
	using closure = std::function<void(Q...)>;

	property(closure go) : basic(close(go)) { };

private:

	using basic::basic;

	static auto close(closure go)
	{
		return [=](Q... q)
		{
			go(q...); 
			return P; 
		};
	}
};

template <typename... Q> using none = property<false, Q...>;
template <typename... Q> using total = property<true, Q...>;

template <typename... Q>
constexpr auto never = [](Q...) { return false; };
template <typename... Q> 
constexpr auto always = [](Q...) { return true; };

#endif // file
