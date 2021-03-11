#ifndef doc_hpp
#define doc_hpp "Document Structure"

#include "opt.hpp"

namespace doc
{
	using name  = env::opt::name;
	using pair  = env::opt::pair;
	using graph = fwd::graph<name>;
	using group = fwd::group<name>;
	using edges = fwd::edges<name>;

	template <class C> auto table(const C* = nullptr)
	{
		return C::table;
	}
}

#endif // file
