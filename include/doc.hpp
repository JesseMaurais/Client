#ifndef doc_hpp
#define doc_hpp "Document Structure"

#include "fwd.hpp"
#include "tmp.hpp"
#include "ptr.hpp"
#include <typeindex>

namespace doc
{
	struct interface : fwd::no_copy, fmt::layout<interface>
	{
		virtual std::type_index type() const = 0;
		virtual std::ptrdiff_t size() const = 0;
		virtual fwd::span<int> at(int n) const = 0;
		virtual bool contains(int n) const = 0;
		virtual void destroy(int n) = 0;
	};

	template <class Type> class instance : final interface
	{
		std::type_index type() const override;
		std::ptrdiff_t size() const override;
		fwd::span<int> at(int n) const override;
		bool contains(int n) const override;
		void destroy(int n) override;

		instance();

	public:

		using write_ptr = fwd::shared_ptr<Type>;
		using read_ptr = fwd::shared_ptr<const Type>;

		write_ptr writer(int n);
		read_ptr reader(int n);
		int emplace(Type &&);

		static ref self();
	};

	using manager = instance<doc::interface::ptr>;
	extern template class manager;

	using strings = instance<fmt::string::view>;
	extern template class strings;

	using message = instance<fwd::function>;
	extern template class message;

	static auto signal(fwd::function f)
	{
		return message::self().emplace(f);
	}

	static auto cancel(int n)
	{
		return message::self().destroy(n);
	}

	static auto raise(int n)
	{
		return message::self()->reader()->operator()();
	}
}

#endif // file
