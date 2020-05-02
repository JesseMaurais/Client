#ifndef err_hpp
#define err_hpp "Error Format"

#include <system_error>
#include <exception>
#include <sstream>
#include "fmt.hpp"

// Pre condition

#ifdef assert
# undef assert
# warning You should not include assert
#endif

// Post conditions

#ifdef NDEBUG
# define verify(x) (x)
# define assert(x)
# define alert(x) 
# define trace(...)
#else
# define verify(x) assert(x)
# define assert(x) if (not(x)) sys::warn(here, #x)
# define alert(x) if (bool(x)) sys::err(here, #x)
# define trace(...) sys::warn(here, __VA_ARGS__)
# include "bug.hpp"
#endif

#define here { __FILE__, __LINE__, __func__ }

// Error format

namespace fmt
{
	struct where // the state of here
	{
		char const *file; 
		long const line; 
		char const *call;
	};

	string::out operator<<(string::out, where const &);
	string::out operator<<(string::out, std::errc const &);
	string::out operator<<(string::out, std::exception const &);

	template <typename T, typename... S> auto err(T t, S... s)
	{
		string::str ss; ss << t;
		if constexpr (0 < sizeof...(s)) ((ss << ' ' << s), ...);
		return ss.str();
	}
}

// Error pipe

namespace sys
{
	extern bool debug;

	namespace impl
	{
		int warn(fmt::string::view);
		int err(fmt::string::view);
	}

	template <typename... T> inline int warn(T... t)
	{
		return
		#ifndef NDEBUG
			debug ? impl::warn(fmt::err(t...)) :
		#endif
			-1;
	}

	template <typename... T> inline int err(T... t)
	{
		return
		#ifndef NDEBUG
			debug ? impl::err(fmt::err(t...)) :
		#endif
			-1;
	}
}

#endif // file
