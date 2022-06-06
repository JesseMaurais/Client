#ifndef xauth_hpp
#define xauth_hpp

#include "fmt.hpp"

namespace x11
{
	fmt::view authority();
}

namespace x11::auth
{
	struct info : fmt::memory<info>
	{
		unsigned short family;
		fmt::string address;
		fmt::string number;
		fmt::string name;
		fmt::string data;
	};

	fmt::input operator>>(fmt::input, info::ref);
};

#endif
