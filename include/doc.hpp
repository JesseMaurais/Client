#ifndef doc_hpp
#define doc_hpp "Document Structure"

#include "tmp.hpp"
#include "ptr.hpp"
#include "fmt.hpp"
#include <tuple>

namespace doc
{
	using path = fmt::diff;

	template <class Type> class instance : fwd::unique
	{
		instance() = default;

		fwd::vector<Type> item;
		fwd::vector<size_t> cross;
		fwd::vector<ptrdiff_t> index;

	public:

		static instance& self();
		int open(Type&&);
		size_t close(int);
		Type* find(int);
		Type& at(int);

		inline auto gap() const
		{
			return index.size() - item.size();
		}
	};

	template <class Type> auto& access()
	{
		return instance<Type>::self();
	}

	template <auto K> static fmt::string::view name = "(none)";

	template <class C> constexpr auto table(const C* = nullptr)
	{
		return C::table();
	}

	template <size_t N, class C> constexpr auto get(const C* = nullptr)
	{
		return std::get<N>(table<C>());
	}

	template <size_t N, class C> fmt::string::view key(const C* = nullptr)
	{
		return name<get<N, C>()>;
	}

	template <size_t N, class C> auto& value(const C* that)
	{
		return that->*get<N>(that);
	}

	template <size_t N, class C> auto& value(C* that)
	{
		return that->*get<N>(that);
	}
}

#endif // file
