#ifndef err_hpp
#define err_hpp "Error Format"

#include "fmt.hpp"

// Pre condition

#ifdef assert
#	undef assert
#	warning You should not include assert
#endif

// Post conditions

#define here fmt::where { __FILE__, __LINE__, __func__ }

#ifndef NDEBUG
#	define assert(x) if (not(x)) sys::warn(here, #x)
#	define alert(x) if (bool(x)) sys::err(here, #x)
#	define trace(...) sys::warn(here, __VA_ARGS__)
#	define verify(x) assert(x)
#else
#	define assert(x)
#	define alert(x) 
#	define trace(...)
#	define verify(x) (x)
#endif

// Negative boolean

enum : bool { success = false, failure = true };

static_assert(failure != success);

// Error format

namespace fmt
{
	struct where { const char* file; int line; const char* func; };

	template <typename... T> auto err(where const& w, T... t)
	{
		string::stream ss;
		ss 
			<< w.file 
			<< " (" 
			<< w.line 
			<< ") " 
			<< w.func 
			<< ": ";

		(void) (ss << ...  << t);
		return ss.str();
	}
}

// Error pipe

namespace sys
{
	extern fmt::string::out::ref out; // Output device
	extern fmt::string::in::ref in; // Input device

	namespace impl
	{
		int bug(fmt::string::view, bool);
	}

	extern bool debug; // Whether to write out

	template <typename... T> int warn(T... t)
	{
		return debug ? impl::bug(fmt::err(t...), false) : -1;
	}

	template <typename... T> int err(T... t)
	{
		return debug ? impl::bug(fmt::err(t...), true) : -1;
	}
}

// Unit tests

#ifndef NDEBUG
#	include "bug.hpp"
#endif

#endif // file
