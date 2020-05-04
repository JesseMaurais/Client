#ifndef err_hpp
#define err_hpp "Error Format"

#include <system_error>
#include <exception>
#include "fmt.hpp"

#define where { __FILE__, "__LINE__", __func__ }
#define here fmt::string::view::init(where)

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
#endif

// Error format

namespace fmt
{
	string::out operator<<(string::out, std::errc const &);
	string::out operator<<(string::out, std::exception const &);
	string::out operator<<(string::out, string::view::init);

	template <typename T, typename... S> auto err(T t, S... s)
	{
		string::stream ss; ss << t;
		if constexpr (0 < sizeof...(s)) ((ss << ' ' << s), ...);
		return ss.str();
	}
}

// Error pipe

namespace sys
{
	extern bool debug; // Whether to warn on err
	fmt::string::out::ref out(int); // Thread local output device
	void flush(int); // Write out all at pipe to error

	namespace impl
	{
		int bug(fmt::string::view, bool);
	}

	template <typename... T> inline int warn(T... t)
	{
		return
		#ifndef NDEBUG
			debug ? impl::bug(fmt::err(t...), false) :
		#endif
			-1;
	}

	template <typename... T> inline int err(T... t)
	{
		return
		#ifndef NDEBUG
			debug ? impl::bug(fmt::err(t...), true) :
		#endif
			-1;
	}
}

#ifndef NDEBUG
# include "bug.hpp"
#endif

#endif // file
