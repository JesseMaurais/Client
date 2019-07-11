#ifndef err_hpp
#define err_hpp

#include "dbg.hpp"
#include <sstream>
#include <string>
#include <cstdio>

namespace sys
{
	template <typename... Args> void perror(Args... args)
	{
		if constexpr (DEBUG)
		{
			std::stringstream stream;
			((stream << args << " "), ...);
			if (stream)
			{
				std::perror(stream.str().c_str());
			}
		}
	}
}

#endif // file
