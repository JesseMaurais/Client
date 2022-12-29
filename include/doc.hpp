#ifndef doc_hpp
#define doc_hpp "Document Object Model"

#include "fwd.hpp"
#include "tmp.hpp"
#include "ptr.hpp"
#include <typeindex>
#include <type_traits>
#include <string_view>

#ifdef interface
#undef interface // struct in "combaseapi.h"
#endif

namespace doc
{
	struct interface : fwd::no_copy
	{
		virtual std::type_index type() const = 0;
		virtual std::ptrdiff_t size() const = 0;
		virtual std::span<const int> link(int n) const = 0;
		virtual bool contains(int n) const = 0;
		virtual void destroy(int n) = 0;
	};

	interface* find(std::type_index);

	template <class Type> struct instance final : interface
	{
		using write_ptr = fwd::shared_ptr<Type>;
		using read_ptr = fwd::shared_ptr<const Type>;

		std::type_index type() const override;
		std::ptrdiff_t size() const override;
		std::span<const int> link(int n) const override;
		bool contains(int n) const override;
		void destroy(int n) override;

		write_ptr writer(int n);
		read_ptr reader(int n);
		int emplace(Type &&);

		static instance& self();

	private:

		instance();
	};
	
	extern template struct instance<fwd::event>;
	using event = instance<fwd::event>;

	inline auto signal(fwd::event f)
	{
		return event::self().emplace(std::move(f));
	}

	inline auto raise(int n)
	{
		return event::self().reader(n)->operator()();
	}

	inline auto cancel(int n)
	{
		return event::self().destroy(n);
	}

	template <class C> constexpr auto table(const C *that = nullptr)
	{
		return that->table; // std::tuple of pointers to members of C
	}

	template <size_t N, class C> constexpr auto index(const C *that = nullptr)
	{
		return std::get<N>(table(that)); // the N'th member of C's table
	}

	template <size_t N, class C> inline auto& get(const C* that)
	{
		return that->*index<N>(that); // read from the N'th member of C
	}

	template <size_t N, class C> inline auto& set(C* that)
	{
		return that->*index<N>(that); // write to the N'th member of C
	}

	template <auto K> extern std::string_view name;

	template <size_t N, class C> inline auto key(const C* = nullptr)
	{
		return name<index<N, C>()>; // name of the N'th member of C's table
	}
}

#endif // file
