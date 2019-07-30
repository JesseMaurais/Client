#ifndef ptr_hpp
#define ptr_hpp

#include <memory>

namespace
{
	template <typename Type, typename Remove>
	inline auto make_ptr(Type* ptr, Remove rm)
	{
		return std::unique_ptr<Type, Remove>(ptr, rm);
	}

	template <typename Type, typename Remove>
	inline auto null_ptr(Remove rm)
	{
		return make_ptr((Type) nullptr, rm);
	}
}

#endif // file
