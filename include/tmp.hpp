#ifndef tmp_hpp
#define tmp_hpp "Template Functions"

#include <string_view>
#include <functional>
#include <utility>

namespace fwd
{
	template <class T> auto equal_to(T v)
	{
		return [v](T u) { return u==v; };
	}

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

	template <class T> using defer = std::function<T(void)>;

	using event = defer<void>;

	struct pop : event
	{
		using event::event;

		~pop()
		{
			event::operator()();
		}
	};

	template <class T> struct lazy : defer<T>, constant<T>
	{
		using defer = defer<T>;
		using defer::defer;

		operator T() const override
		{
			return defer::operator()();
		}
	};

	#ifndef lazy
	#define lazy(...) lazy<decltype(__VA_ARGS__)>([&]{ return (__VA_ARGS__); })
	#endif

	template <class... Q> constexpr auto always = [](Q...) { return true; };
	template <class... Q> constexpr auto never = [](Q...) { return false; };

	template <class... T> struct formula : std::function<bool(T...)>
	{
		using function = std::function<bool(T...)>;
		using function::function;

		formula operator and(const function &that) const
		{
			return [&](T... x)
			{
				return (*this)(x...) and that(x...);
			};
		}

		formula operator or(const function &that) const
		{
			return [&](T... x)
			{
				return (*this)(x...) or that(x...);
			};
		}

		formula operator xor(const function &that) const
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
}

#endif // file
