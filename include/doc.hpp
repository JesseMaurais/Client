#ifndef doc_hpp
#define doc_hpp "Document Object Model"

#include "fwd.hpp"
#include "tmp.hpp"
#include "ptr.hpp"
#include <typeindex>
#include <string_view>

namespace doc
{
	struct interface : fwd::no_copy
	{
		virtual std::type_index type() const = 0;
		virtual std::ptrdiff_t size() const = 0;
		virtual std::span<int> link(int n) const = 0;
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
		std::span<int> link(int n) const override;
		bool contains(int n) const override;
		void destroy(int n) override;

		write_ptr writer(int n);
		read_ptr reader(int n);
		int emplace(Type &&);

		static instance& self();

	private:

		instance();
	};

	extern template struct instance<fwd::function>;
	using message = instance<fwd::function>;

	inline auto signal(fwd::function f)
	{
		return message::self().emplace(std::move(f));
	}

	inline auto raise(int n)
	{
		return message::self().reader(n)->operator()();
	}

	inline auto cancel(int n)
	{
		return message::self().destroy(n);
	}
}

#endif // file
