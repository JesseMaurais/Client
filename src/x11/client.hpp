#ifndef xclient_hpp
#define xclient_hpp

#include "str.hpp"
#include "x11.hpp"
#include <iosfwd>

namespace x11
{
	fmt::string setup
	/// Server handshake, returns error message
	(
		std::iostream& io, 
		fmt::string_view proto, 
		fmt::string_view data
	);
}

#endif
