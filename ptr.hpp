#ifndef ptr_hpp
#define ptr_hpp

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

template <typename Type, typename Vector>
inline auto && emplace(Vector & t, Type & d)
{
	t.emplace_back(std::move(d));
	return t.back();
}

class unique
{
	unique(const unique &) = delete;
	unique & operator = (unique const &) = delete;
protected:
	unique() = default;
};

#endif // file
