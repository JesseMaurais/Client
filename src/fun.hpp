#ifndef fun_hpp
#define fun_hpp

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

template <typename... T>
constexpr auto falsity = [](T...) { return false; };
template <typename... T>
constexpr auto truth = [](T...) { return true; };

#endif // file
