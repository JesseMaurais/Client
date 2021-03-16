#ifndef doc_hpp
#define doc_hpp "Document Structure"

#include "tmp.hpp"
#include "opt.hpp"
#include <tuple>

namespace doc
{
	template <class C> auto table(const C* = nullptr)
	{
		return C::table();
	}

	template <size_t N, class C> auto get(const C* = nullptr)
	{
		return std::get<N>(table<C>());
	}

	template <size_t N, size_t M, class C> auto get(const C* = nullptr)
	{
		return std::get<M>(get<N, C>());
	}

	template <size_t N, class C> fmt::string::view key(const C* = nullptr)
	{
		return get<N, 1, C>();
	}

	template <size_t N, class C> fmt::string::view key(const C&)
	{
		return get<N, 1, C>();
	}

	template <size_t N, class C> auto& value(const C* that)
	{
		return that->*get<N, 0>(that);
	}

	template <size_t N, class C> auto& value(C* that)
	{
		return that->*get<N, 0>(that);
	}

	template <size_t N, class C> auto& value(const C& that)
	{
		return that.*get<N, 0>(&that);
	}

	template <size_t N, class C> auto& value(C& that)
	{
		return that.*get<N, 0>(&that);
	}

	struct node : fmt::struct_brief<env::opt::pair>
	{
		type index;
		vector list;
	};
}

#endif // file
