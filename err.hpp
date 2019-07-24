#ifndef err_hpp
#define err_hpp

#ifdef NDEBUG
# define verify(x) (x)
#else
# define verify(x) assert(x)
# include <sstream>
# include <cstdio>
# include <cassert>
namespace fmt
{
	template <typename Arg, typename... Args>
	inline auto error(Arg init, Args... args)
	{
		std::stringstream ss;
		ss << init;
		((ss << ": " << args), ...);
		return ss.str();
	}
}
#endif

namespace sys
{
	template <typename... Args> inline void perror(Args... args)
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

// Compile error on std::perror use -- switch to sys::perror
#ifndef NDEBUG
# define inplace __FILE__, __LINE__, __FUNCTION__
# define perror(...) perror(inplace, __VA_ARGS__)
#else
# define inplace
#endif

#endif // file
