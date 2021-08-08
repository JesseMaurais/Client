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
		int open(Type&);
		int close(int);
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

	template <class C> constexpr auto table(const C *that = nullptr)
	{
		return that->table();
	}

	template <size_t N, class C> constexpr auto index(const C *that = nullptr)
	{
		return std::get<N>(table(that));
	}

	template <size_t N, class C> auto& get(const C* that)
	{
		return that->*index<N>(that);
	}

	template <size_t N, class C> auto& set(C* that)
	{
		return that->*index<N>(that);
	}

	template <auto K> static fmt::string::view name = "(none)";

	template <size_t N, class C> fmt::string::view key(const C* = nullptr)
	{
		return name<index<N, C>()>;
	}
}

#endif // file
