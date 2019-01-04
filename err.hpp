#ifndef err_hpp
#define err_hpp

#include <sstream>
#include <string>
#include <cstdio>
#include "fmt.hpp"

namespace sys
{
	template <typename... Args>
	void ferror(fmt::string_view format, Args... args)
	{
		auto const s = (fmt::format(format) % ... % args);
		std::perror(s.c_str());
	}

	template <typename... Args>
	void perror(Args... args)
	{
		std::stringstream stream;
		((stream << args << " "), ...);
		if (stream)
		{
			std::perror(stream.str().c_str());
		}
	}
}

#endif // file
