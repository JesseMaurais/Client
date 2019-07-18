#ifndef err_hpp
#define err_hpp

constexpr bool DEBUG =
#ifdef NDEBUG
	false;
# define verify(x) (x)
#else
	true;
# define verify(x) assert(x)
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
		#ifndef NDEBUG
		{
			auto s = fmt::error(args...);
			if (not empty(s))
			{
				std::perror(s.c_str());
			}
		}
		#endif
	}
}

#endif // file
