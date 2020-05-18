#ifndef ptr_hpp
#define ptr_hpp "Object Pointer"

#include <memory>

namespace fwd
{
	struct unique
	{
		unique(const unique &) = delete;
		unique & operator = (unique const &) = delete;
	protected:
		unique() = default;
	};

	template 
	<
		class Type, class Remove
	>
	auto make_ptr(Type* ptr, Remove rm)
	{
		return std::unique_ptr<Type, Remove>(ptr, rm);
	}

	template 
	<
		class Type, class Remove
	>
	auto null_ptr(Remove rm)
	{
		return make_ptr((Type) nullptr, rm);
	}
}

#endif // file
