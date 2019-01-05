#ifndef any_hpp
#define any_hpp

#if __has_include(<any>)
#include <any>
namespace stl
{
	using any = std::any;
}
#elif __has_include(<experimental/any>)
#include <experimental/any>
namespace stl
{
	using any = std::experimental::any;
};
#elif __has_include(<boost/any.hpp>)
#include <boost/any.hpp>
namespace stl
{
	using any = boost::any;
};
#else
#error Cannot find an implementation of std::any.
#endif

#endif // file
