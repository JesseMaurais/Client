#ifndef tmp_hpp
#define tmp_hpp "Template Meta Programming"

#include <type_traits>
#include <functional>
#include <iterator>
#include <utility>
#include <cstddef>

namespace fwd
{
	template <auto f> class offset_of
	{
		static_assert(std::is_member_object_pointer<decltype(f)>::value);

		template <class C, class T> static std::pair<C, T> null(T C::*);

	public:
	
		// type deduction
		using pair_type = decltype(null(f));
		using value_type = typename pair_type::first_type;
		using parent_type = typename pair_type::second_type;
	};

	template <class T> struct iterator
	{
		using F = std::function<T(T)>;

		iterator(T size, F step) 
		: pos(size), next(step)
		{ }

		auto operator++()
		{
			return pos = next(pos);
		}

		auto operator*()
		{
			return pos;
		}

	private:

		T pos;
		F next;
	};

	template <class T> class range : std::pair<T, T>
	{
		using I = iterator<T>;
		using F = typename I::F;
		using P = std::pair<T, T>;

		F next;

	public:

		range(T start, T finish, F step)
		: P(start, finish), next(step)
		{ 
			#ifdef assert
			assert(start <= finish);
			#endif
		}

		auto begin()
		{
			return I(this->first, next);
		}

		auto end()
		{
			return I(this->second, next);
		}
	};

	template <class T> auto up_to(T size, T from = 0, T by = 1)
	{
		#ifdef assert
		assert(0 == ((size - from) % by));
		#endif
		return range<T> 
		{ 
			from, size, [by](T pos)
			{
				return std::next(pos, by);
			}
		};
	}

	template <class T> auto down_from(T size, T to = 0, T by = 1)
	{
		#ifdef assert
		assert(0 == ((size - to) % by));
		#endif
		return range<T> 
		{
			std::prev(to), std::prev(size), [by](T pos)
			{
				return std::prev(pos, by);
			}
		};
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

	template <class... T> struct formula : std::function<bool(T...)>
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

	template <class T> using predicate = formula<T>;

	template <class T, class S> using relation = formula<T, S>;

	template <bool P, class... Q> struct proposition : formula<Q...>
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

	template <class... Q> using truth = proposition<true, Q...>;
	template <class... Q> using falsity = proposition<false, Q...>;

	template <class... Q> constexpr auto never = [](Q...) { return false; };
	template <class... Q> constexpr auto always = [](Q...) { return true; };
}

#endif // file
