#ifndef err_hpp
#define err_hpp "Error Format"

#ifdef assert
# undef assert
# warning You should not include assert. 
#endif

#ifdef NDEBUG
# define verify(x) (x)
# define assert(x)
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

	#define here { __FILE__, __LINE__, __func__ }

	string::out operator<<(string::out, where const &);
	string::out operator<<(string::out, std::errc const &);
	string::out operator<<(string::out, std::exception const &);

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

namespace sys
{
	extern bool debug;

	inline std::errc const noerr { };

	bool iserr(std::errc ec);

	namespace impl
	{
		void warn(fmt::string::view);
		void err(fmt::string::view);
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
