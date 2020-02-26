#ifndef tmp_hpp
#define tmp_hpp // Template Meta Programming

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

template <bool P, typename... Q> struct proposition : formula<Q...>
{
	using base = formula<Q...>;
	using closure = std::function<void(Q...)>;
	using base::base;

	proposition(closure go) : base(enclose(go)) { };

private:

	static auto enclose(closure go)
	{
		return [=](Q... q)
		{
			go(q...);
			return P;
		};
	}
};

template <typename... Q> using contra = proposition<false, Q...>;
template <typename... Q> using taut = proposition<true, Q...>;

template <typename... Q>
constexpr auto never = [](Q...) { return false; };
template <typename... Q>
constexpr auto always = [](Q...) { return true; };

template <typename T> struct property
{
	virtual operator T() const = 0;
};

template <typename T> struct attribute : property<T>
{
	virtual T operator=(T) = 0;
};

#endif // file
