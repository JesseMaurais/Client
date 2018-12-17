#ifndef env_hpp
#define env_hpp

#include <string_view>
#include <utility>
#include <span>

namespace sys::env
{
	std::string_view get(std::string_view s);
	bool put(std::string_view s);
};

namespace sys
{
	std::span<std::string_view> environment();
};

namespace env
{
	std::string_view home();
	std::string_view user();
	std::string_view temp();
	std::string_view shell();
};

#endif // file
