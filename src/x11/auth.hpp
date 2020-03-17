#ifndef xauth_hpp
#define xauth_hpp

#include <iosfwd>
#include "env.hpp"

namespace x11
{
	extern env::view const& authority;
}

namespace x11::auth
{
	struct info
	{
		unsigned short family;
		fmt::string address;
		fmt::string number;
		fmt::string name;
		fmt::string data;
	};

	std::istream& operator>>(std::istream& in, info& out);
};

#endif
