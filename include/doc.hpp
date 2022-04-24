#ifndef doc_hpp
#define doc_hpp "Document Structure"

#include "tmp.hpp"
#include "ptr.hpp"
#include "fmt.hpp"
#include <typeindex>
#include <tuple>

namespace doc
{
	using node = fmt::layout<int>;

	// Abstract access to classes

	struct interface : fwd::no_copy
	{
		virtual node::span next(node::type) const = 0;
		virtual std::type_index type() const = 0;
		virtual std::size_t size() const = 0;
	};

	template <class Type> class instance : final interface
	{
		node::span next(node::type) const override;
		std::type_index type() const override;
		std::size_t size() const override;

		instance();

	public:

		static instance& self();

		using read_ptr = fwd::shared_ptr<const Type>;
		using write_ptr = fwd::shared_ptr<Type>;

		read_ptr read(int);
		write_ptr write(int);
		int emplace(Type &&);
		bool contains(int);
		void erase(int);
	};

	// Abstract access to members of classes

	template <class C> constexpr auto table(const C *that = nullptr)
	{
		return that->table(); // std::tuple of pointers to members of C
	}

	template <size_t N, class C> constexpr auto index(const C *that = nullptr)
	{
		return std::get<N>(table(that)); // the N'th member of C's table
	}

	template <auto K> extern fmt::view name;

	template <size_t N, class C> inline auto key(const C* = nullptr)
	{
		return name<index<N, C>()>; // name of the N'th member of C's table
	}

	template <size_t N, class C> inline auto& get(const C* that)
	{
		return that->*index<N>(that); // read the N'th member of C
	}

	template <size_t N, class C> inline auto& set(C* that)
	{
		return that->*index<N>(that); // write to the N'th member of C
	}
}

#endif // file
