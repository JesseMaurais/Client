#ifndef doc_hpp
#define doc_hpp "Document Structure"

#include "tmp.hpp"
#include "opt.hpp"
#include <tuple>

namespace doc
{
	template <class Type> class instance : fwd::unique
	{
		fwd::vector<ptrdiff_t> index, cross;
		fwd::vector<Type> item;
		instance() = default;

	public:

		static instance& self();
		size_t emplace(Type&&);
		size_t erase(size_t);
		Type* find(size_t);
		Type& at(size_t);

		inline auto gap() const
		{
			return index.size() - item.size();
		}
	};

	template <class Type> auto& access()
	{
		extern template class instance<Type>;
		return instance<Type>::self();
	}

	template <class Type> class access_ptr : fwd::unique
	{
		size_t const pos;

	public:

		access_ptr(Type&& type) : pos(access<Type>().emplace(type))
		{ }

		~access_ptr()
		{
			access<Type>().erase(pos);
		}

		auto operator->() const
		{
			return &access<Type>().at(pos);
		}

		auto operator->()
		{
			return &access<Type>().at(pos);
		}
	};

	struct node : fmt::struct_brief<env::opt::pair>
	{
		type index;
		vector list;
	};

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
}

#endif // file
