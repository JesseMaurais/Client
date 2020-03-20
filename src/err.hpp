#ifndef err_hpp
#define err_hpp

#ifdef assert
# undef assert
# warning You should not include assert. 
#endif

#ifdef NDEBUG
# define assert(x)
# define verify(x) (x)
# define alert(x) 
# define debug(x) if constexpr (false)
#else
# define verify(x) assert(x)
# define assert(x) if (not(x)) sys::warn(here, #x)
# define alert(x) if (bool(x)) sys::err(here, #x)
# define debug(x) if (x)
#endif

#include <system_error>
#include <exception>
#include <sstream>
#include "fmt.hpp"

// Pessimistic boolean

enum : bool
{
	success = false, failure = true
};

namespace fmt
{
	struct where
	{
		char const *file; int const line; char const *func;
	};

	fmt::string::out operator<<(fmt::string::out, where const &);
	fmt::string::out operator<<(fmt::string::out, std::errc const &);
	fmt::string::out operator<<(fmt::string::out, std::exception const &);

	template <typename Arg, typename... Args>
	auto err(Arg arg, Args... args)
	{
		std::stringstream ss;
		ss << arg;
		if constexpr (0 < sizeof...(args))
		{
			((ss << " " << args), ...);
		}
		return ss.str();
	}
}

#define here ::fmt::where { __FILE__, __LINE__, __func__ }

namespace sys
{
	extern bool debug;

	inline std::errc const noerr { };

	bool iserr(std::errc ec);

	namespace impl
	{
		void warn(fmt::string_view);
		void err(fmt::string_view);
	}

	template <typename... Args> inline bool warn(Args... args)
	{
		#ifndef NDEBUG
		if (debug)
		{
			impl::warn(fmt::err(args...));
			return success;
		}
		#endif
		return failure;
	}

	template <typename... Args> inline bool err(Args... args)
	{
		#ifndef NDEBUG
		if (debug)
		{
			impl::err(fmt::err(args...));
			return success;
		}
		#endif
		return failure;
	}
}

#endif // file
