#ifndef xauth_hpp
#define xauth_hpp

#include <iosfwd>
#include "env.hpp"
#include "x11.hpp"

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

	bytes::in::ref operator>>(bytes::in::ref, info::ref);
};

#endif
