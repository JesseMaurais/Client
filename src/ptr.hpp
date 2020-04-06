#ifndef ptr_hpp
#define ptr_hpp "Object Pointer"

#include <memory>

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

class unique
{
	unique(const unique &) = delete;
	unique & operator = (unique const &) = delete;
protected:
	unique() = default;
};

#endif // file
