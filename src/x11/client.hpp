#ifndef xclient_hpp
#define xclient_hpp

#include "str.hpp"
#include "x11.hpp"

namespace x11
{
	fmt::string socket(int screen);
	int open(fmt::string_view dpy);
}

#endif
