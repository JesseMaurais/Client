#ifndef ptr_hpp
#define ptr_hpp

#include <memory>

namespace
{
	template <typename Type, typename Remove>
	inline auto make(Type* ptr, Remove rm)
	{
		return std::unique_ptr<Type, Remove>(ptr, rm);
	}

	template <typename Type, typename Result>
	inline auto null(Result(*rm)(Type*))
	{
		return make<Type>(nullptr, rm);
	}

	template <typename Type>
	using shared = std::enable_shared_from_this<Type>;
}

#endif // file
