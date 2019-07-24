#ifndef ptr_hpp
#define ptr_hpp

#include <memory>

namespace ptr
{
	template <typename Type, typename Remove>
	auto make(Type* ptr, Remove rm)
	{
		return std::unique_ptr<Type, Remove>(ptr, rm);
	}
}

#endif // file
