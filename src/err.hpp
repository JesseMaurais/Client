#ifndef err_hpp
#define err_hpp "Error Format"

#ifdef assert
# undef assert
# warning You should not include assert
#endif

#ifdef here
# undef here
# warning You should not define word here
#endif
#define here { __FILE__, __LINE__, __func__ }

#ifdef NDEBUG
# define verify(x) (x)
# define assert(x)
# define alert(x) 
# define trace(...)
# define debug(x) if constexpr (false)
#else
# define verify(x) assert(x)
# define assert(x) if (not(x)) sys::warn(here, #x)
# define alert(x) if (bool(x)) sys::err(here, #x)
# define trace(...) sys::warn(here, __VA_ARGS__)
# define debug(x) if (x)
#endif

#include <system_error>
#include <exception>
#include <sstream>
#include "fmt.hpp"

namespace fmt
{
	struct where // the state of here
	{
		char const *file; int const line; char const *func;
	};

	string::out operator<<(string::out, where const &);
	string::out operator<<(string::out, std::errc const &);
	string::out operator<<(string::out, std::exception const &);

	template <typename Arg, typename... Args>
	auto err(Arg arg, Args... args)
	{
		string::str ss;
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
	enum : bool { success = false, failure = true };

	extern bool debug;

	inline std::errc const noerr { };

	bool iserr(std::errc ec);

	namespace impl
	{
		int warn(fmt::string::view);
		int err(fmt::string::view);
	}

	template <typename... Args> inline int warn(Args... args)
	{
		#ifndef NDEBUG
		if (debug)
		{
			return impl::warn(fmt::err(args...));
		}
		#endif
		return -1;
	}

	template <typename... Args> inline int err(Args... args)
	{
		#ifndef NDEBUG
		if (debug)
		{
			return impl::err(fmt::err(args...));
		}
		#endif
		return -1;
	}
}

#endif // file
