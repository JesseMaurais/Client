#ifndef any_hpp
#define any_hpp

#if __has_include(<any>)
#include <any>
namespace stl
{
	using any = std::any;
	using bad_any_cast = std::bad_any_cast;
	template <typename T> constexpr auto any_cast = std::any_cast;
}
#elif __has_include(<experimental/any>)
#include <experimental/any>
namespace stl
{
	using any = std::experimental::any;
	using bad_any_cast = std::experimental::bad_any_cast;
	template <typename T> constexpr auto any_cast = std::experimental::any_cast;
};
#elif __has_include(<boost/any.hpp>)
#include <boost/any.hpp>
namespace stl
{
	using any = boost::any;
	using bad_any_cast = boost::bad_any_cast;
	template <typename T> constexpr auto any_cast = boost::any_cast;
};
#else
#error Cannot find an implementation of std::any.
#endif

#endif // file
