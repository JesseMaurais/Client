#ifndef tmp_hpp
#define tmp_hpp "Template Meta Programming"

#include <functional>
#include <utility>

namespace fwd
{
	template <class T> struct type_of
	// The $type of class $T
	{
		using type = T;
	};

	template <class T> struct evaluator : type_of<T>
	// A predicate operator on class $T
	{
		virtual bool operator()(T) const = 0;
	};

	template <class T> struct constant : type_of<T>
	// A read operator on class $T
	{
		virtual operator T() const = 0;
	};

	template <class T> struct variable : constant<T>
	// A read & write operator on class $T
	{
		virtual T operator=(T) = 0;
	};

	template <class T> using function = std::function<T(T)>;
	// A function that is closed on class $T

	template <class T> using notify = std::function<void(T)>;
	// A function to notify with class $T

	template <class T> using defer = std::function<T(void)>;
	// A function to defer evaluation of class $T

	template <class T> struct lazy : defer<T>, constant<T>
	// On demand evaluation of class $T
	{
		using defer = defer<T>;
		using defer::defer;

		operator T() const override
		{
			return defer::operator()();
		}
	};

	using event = defer<void>;

	inline auto raise(event signal)
	{
		return std::invoke(signal);
	}

	struct pop : event
	// Raise on return
	{
		using event::event;

		~pop()
		{
			event::operator()();
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

#ifndef _lazy
#define _lazy(...) fwd::lazy([=]{ return __VA_ARGS__; })
#endif

#ifndef _pop
#define _pop(...) fwd::pop([=]{ return __VA_ARGS___; })
#endif

#endif // file
