#ifndef tab_hpp
#define tab_hpp "Document Table Access"

#include <string_view>
#include <utility>

namespace doc
{
	template <class C> constexpr auto table(const C *that = nullptr)
	{
		return that->table; // std::tuple of pointers to members of C
	}

	template <size_t N, class C> constexpr auto index(const C *that = nullptr)
	{
		return std::get<N>(table(that)); // the N'th member of C's table
	}

	template <size_t N, class C> inline auto& get(const C* that)
	{
		return that->*index<N>(that); // read from the N'th member of C
	}

	template <size_t N, class C> inline auto& set(C* that)
	{
		return that->*index<N>(that); // write to the N'th member of C
	}

	template <auto K> extern std::string_view name;

	template <size_t N, class C> inline auto key(const C* = nullptr)
	{
		return name<index<N, C>()>; // name of the N'th member of C's table
	}
}

#endif