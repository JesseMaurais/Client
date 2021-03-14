#ifndef doc_hpp
#define doc_hpp "Document Structure"

#include "tmp.hpp"
#include "opt.hpp"

using namespace std::literals::string_view_literals;

namespace doc
{
	using pair = fwd::pair<env::opt::name>;
	using group = fwd::group<env::opt::name>;

	template <class C> auto table(const C* = nullptr)
	{
		return C::table();
	}

	template <size_t N, class C> auto get(const C* = nullptr)
	{
		return std::get<N>(table<C>());
	}

	template <size_t M, size_t N, class C> auto get(const C* = nullptr)
	{
		return std::get<N>(get<M, C>());
	}

	template <size_t M, size_t N, class C> auto& value(const C* that)
	{
		return that->*get<M, N>(that);
	}

	template <size_t M, size_t N, class C> auto& value(C* that)
	{
		return that->*get<M, N>(that);
	}

	enum index : size_t
	{
		data, name
	};
}

#endif // file
