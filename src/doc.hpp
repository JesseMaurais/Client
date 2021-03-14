#ifndef doc_hpp
#define doc_hpp "Document Structure"

#include "tmp.hpp"
#include "opt.hpp"
#include <tuple>

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
		return that.*get<N, 0, C>();
	}

	template <size_t N, class C> auto& value(C& that)
	{
		return that.*get<N, 0, C>();
	}
}

#endif // file
