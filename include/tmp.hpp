#ifndef tmp_hpp
#define tmp_hpp "Template Meta Programming"

#include <functional>
#include <utility>

#define LAZY(...) ::fwd::lazy([=]{ return __VA_ARGS__; })

namespace fwd
{
	template <class T> struct type_of
	{
		using type = T;
	};

	template <class T> struct evaluator : type_of<T>
	{
		virtual bool operator()(T) const = 0;
	};

	template <class T> struct constant : type_of<T>
	{
		virtual operator T() const = 0;
	};

	template <class T> struct variable : constant<T>
	{
		virtual T operator=(T) = 0;
	};

	template <class T> using function = std::function<T(T)>;
	template <class T> using notify = std::function<void(T)>;
	template <class T> using query = std::function<T(void)>;
	using event = std::function<void()>;

	template <class T> struct lazy : query<T>, constant<T>
	{
		using query = query<T>;
		using query::query;

		operator T() const override
		{
			return query::operator()();
		}
	};

	struct scope : query<void>
	{
		using query = query<void>;
		using query::query;

		~scope()
		{
			query::operator()();
		}
	};

	template <class... T> struct formula : std::function<bool(T...)>
	{
		using function = std::function<bool(T...)>;
		using function::function;

		formula operator and(function const& that) const
		{
			return [&](T... x)
			{
				return (*this)(x...) and that(x...);
			};
		}

		formula operator or(function const& that) const
		{
			return [&](T... x)
			{
				return (*this)(x...) or that(x...);
			};
		}

		formula operator xor(function const& that) const
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

	template <class T> using predicate = formula<T>;

	template <class T, class S=T> using relation = formula<T, S>;

	using sentence = predicate<void>;

	template <bool P, class... Q> struct proposition : formula<Q...>
	{
		using formula = formula<Q...>;
		using formula::formula;
		using closure = std::function<void(Q...)>;

		proposition(closure go) : formula(enclose(go)) { };

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

	template <class... Q> using always = proposition<true, Q...>;
	template <class... Q> using never = proposition<false, Q...>;

	template <class... Q> constexpr auto falsity = [](Q...) { return false; };
	template <class... Q> constexpr auto truth = [](Q...) { return true; };
}

#endif // file
