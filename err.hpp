#ifndef err_hpp
#define err_hpp

constexpr bool DEBUG =
#ifdef NDEBUG
# define verify(x) (x)
	false;
#else
# define verify(x) assert(x)
	true;
# include <sstream>
# include <cstdio>
# include <cassert>
namespace fmt
{
	template <typename... Args> auto error(Args... args)
	{
		std::stringstream ss;
		((ss << args << ": "), ...);
		return ss.str();
	}
}
#endif

namespace sys
{
	template <typename... Args> void perror(Args... args)
	{
		if constexpr (DEBUG)
		{
			auto s = fmt::error(args...);
			if (not empty(s))
			{
				std::perror(s.c_str());
			}
		}
	}
}

#endif // file
