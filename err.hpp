#ifndef err_hpp
#define err_hpp

#define here __FILE__, __LINE__, __func__

#ifdef assert
# undef assert
# warning You should not include assert. 
#endif

#ifdef NDEBUG
# define assert(x)
# define verify(x) (x)
#else
# define verify(x) assert(x)
# define assert(x) if (not(x)) sys::warn(here, #x, '\n');
#endif

#include <sstream>
#include <cstdio>

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
	extern bool debug;

	template <typename... Args>
	inline void warn(Args... args)
	{
		if (debug)
		{
			auto const s = fmt::err(args...);
			std::fputs(data(s), stderr);
		}
	}

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

#endif // file
