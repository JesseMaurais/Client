#ifndef bin_hpp
#define bin_hpp "Binary Transform"

#include "dig.hpp"
#include "type.hpp"
#include <tuple>

namespace fmt
{
	template <class N> auto to_bytes(N n)
	{
		auto const w = to_wint(n);
		auto const a = narrow(w);
		auto const begin = a.begin();
		auto const end = a.end();
		return string(begin, end);
	}

	template <class N> auto from_bytes(string::view u)
	{
		auto const a = widen(u);
		auto const begin = a.begin();
		auto const end = a.end();
		auto const w = wstring(begin, end);
		#ifdef assert
		assert(w.size() == 1);
		#endif
		return w.back();
	}

	template <class C> class write
	{
		const C* object;

		template <class T, size_t... I> 
		string::out print(string::out out, const T& members, std::index_sequence<I...>)
		{
			return (..., (out << to_bytes(object->*std::get<I>(members))));
		}

	public:

		write(const C* ptr) : object(ptr) { }
		write(const C& obj) : write(&obj) { }

		friend string::out operator<<(string::out, write);
	};

	template <class C> string::out operator<<(string::out out, write<C> data)
	{
		auto const tuple = serial<C>();
		using type = decltype(tuple):
		constexpr auto size = std::tuple_size<type>::value;
		return data.print(out, tuple, std::make_index_sequence<size>());
	}
}

#endif // file
