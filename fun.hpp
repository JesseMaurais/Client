#ifndef fun_hpp
#define fun_hpp

#include <functional>

template <typename T> struct predicate : std::function<bool(T)>
{
	using base = std::function<bool(T)>;
	using base::base;

	predicate operator and(base const& that) const
	{
		return [&](T x)
		{
			return (*this)(x) and that(x);
		};
	}

	predicate operator or(base const& that) const
	{
		return [&](T x)
		{
			return (*this)(x) or that(x);
		};
	}

	predicate operator xor(base const& that) const
	{
		return [&](T x)
		{
			return (*this)(x) xor that(x);
		};
	}

	predicate operator not() const
	{
		return [&](T x)
		{
			return not (*this)(x);
		};
	}
};

template <typename T> constexpr auto contra = [](T) { return false; };
template <typename T> constexpr auto taut = [](T) { return true; };

#endif // file
