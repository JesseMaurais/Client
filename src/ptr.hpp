#ifndef ptr_hpp
#define ptr_hpp "Object Pointers"

#include <memory>
#include <functional>

namespace fwd
{
	struct unique
	{
		unique(const unique &) = delete;
		unique & operator = (unique const &) = delete;
	protected:
		unique() = default;
	};

	template <class Type> using as_ptr = typename std::add_pointer<Type>::type;
	template <class Type> using as_ref = typename std::add_lvalue_reference<Type>::type;
	template <class Type> using deleter = std::function<void(as_ptr<Type>)>;
	template <class Type> using extern_ptr = std::unique_ptr<Type, deleter<Type>>;

	template 
	<
		class Type, class Remove = deleter<Type>
	>
	auto make_ptr(Type* ptr, Remove free = std::default_delete<Type>())
	{
		return std::unique_ptr<Type, Remove>(ptr, free);
	}

	template 
	<
		class Type, class Remove = deleter<Type>
	>
	auto null_ptr(Remove free = std::default_delete<Type>())
	{
		return make_ptr((Type) nullptr, free);
	}

	template
	<
		class Type, class Remove = deleter<Type>
	>
	struct shared : unique, std::enable_shared_from_this<Type>
	{
		using Type::Type;

		auto extern_from_this(Remove free = std::default_delete<Type>())
		{
			auto ptr = this->shared_from_this();
			auto that = ptr.get();
			ptr.reset();
			return make_ptr(that, free);
		}
	};
}

#endif // file
