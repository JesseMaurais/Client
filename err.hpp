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
	inline auto err(Arg arg, Args... args)
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
	inline void err(Args... args)
	{
		if (debug)
		{
			auto const s = fmt::err(args...);
			std::perror(data(s));
		}
	}
}

#define here __FILE__, __LINE__, __func__

#endif // file
