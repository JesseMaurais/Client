#ifndef doc_hpp
#define doc_hpp "Document Structure"

#include "tmp.hpp"
#include "ptr.hpp"
#include "opt.hpp"
#include <tuple>

namespace doc
{
	using node = fmt::struct_brief<env::opt::pair>;

	template <class Type> class instance : fwd::unique
	{
		instance() = default;
		fwd::vector<Type> item;
		fwd::vector<size_t> cross;
		fwd::vector<ptrdiff_t> index;

	public:

		static instance& self();
		size_t make(Type &&);
		size_t free(size_t);
		Type* find(size_t);
		Type& at(size_t);

		inline auto gap() const
		{
			return index.size() - item.size();
		}
	};

	template <class Type> auto& access()
	{
		return instance<Type>::self();
	}

	template <class Type> class access_ptr : fwd::unique
	{
		size_t const pos;

	public:

		access_ptr(Type&& type) : pos(access<Type>().make(type))
		{ }

		~access_ptr()
		{
			access<Type>().free(pos);
		}

		auto operator->() const
		{
			return access<Type>().find(pos);
		}

		auto operator->()
		{
			return access<Type>().find(pos);
		}

		auto& operator*() const
		{
			return access<Type>().at(pos);
		}

		auto& operator*()
		{
			return access<Type>.at(pos);
		}
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

	template <size_t N, class C> auto& value(const C* that)
	{
		return that->*get<N, 0>(that);
	}

	template <size_t N, class C> auto& value(C* that)
	{
		return that->*get<N, 0>(that);
	}
}

#endif // file
