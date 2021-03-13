#ifndef doc_hpp
#define doc_hpp "Document Structure"

#include "opt.hpp"

namespace doc
{
	template <auto f> class field
	{
		template <class C, class T> static auto make_pair(T C::*)
		{
			constexpr C* obj = nullptr;
			constexpr T* ptr = nullptr;
			return std::make_pair(*obj, *ptr);
		}

	public:

		using pair_type = decltype(make_pair(f));
		using value_type = typename pair_type::second_type;
		using object_type = typename pair_type::first_type;
	};


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
