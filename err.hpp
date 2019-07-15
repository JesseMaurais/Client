#ifndef err_hpp
#define err_hpp

#ifndef NDEBUG
#include <sstream>
#include <cstdio>
#endif

namespace sys
{
	template <typename... Args> void perror(Args... args)
	{
		#ifndef NDEBUG
		{
			std::stringstream stream;
			((stream << args << " "), ...);
			if (stream)
			{
				std::perror(stream.str().c_str());
			}
		}
		#endif
	}
}

#endif // file
