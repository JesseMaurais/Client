#ifndef xdpy_hpp
#define xdpy_hpp

#include "x11/setup.hpp"

namespace x11
{
	fmt::string setup
	(
		fmt::string::io::ref io, 
		fmt::string::view proto, 
		fmt::string::view string
	);
}

#endif
