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

	template <auto K> struct name
	{
		constexpr auto value = K;
	};

	template <auto A> using alias = void;

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
		return alias<get<N, C>()>::value;
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
