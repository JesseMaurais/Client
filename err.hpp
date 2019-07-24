#ifndef err_hpp
#define err_hpp

#ifdef NDEBUG
# define verify(x) (x)
#else
# define verify(x) assert(x)
#endif

#ifndef assert
# include <cassert>
#endif

#include <sstream>
#include <cstdio>

namespace sys
{
	extern bool debug;
}

namespace fmt
{
	template <typename Arg, typename... Args>
	inline auto error(Arg arg, Args... args)
	{
		std::stringstream ss;
		ss << arg;
		((ss << ": " << args), ...);
		return ss.str();
	}
}

namespace sys
{
	template <typename... Args>
	inline void perror(Args... args)
	{
		if (debug)
		{
			auto s = fmt::error(args...);
			if (not empty(s))
			{
				std::perror(s.c_str());
			}
		}
	}
}

#define here __FILE__, __LINE__, __func__

#endif // file
