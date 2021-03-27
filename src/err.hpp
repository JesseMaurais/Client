#ifndef err_hpp
#define err_hpp "Error Format"

#include "fmt.hpp"
#include <sstream>

// Pre condition

#ifdef assert
#	undef assert
#	warning You should not include assert
#endif

// Post conditions

#define here fmt::where { __FILE__, __LINE__, __func__ }

#ifndef NDEBUG
#	ifdef _WIN32
#		define breakpoint() DebugBreak()
#	else
#		define breakpoint() raise(SIGTRAP)
#	endif
#	define assert(...) if (not(__VA_ARGS__)) breakpoint(), sys::warn(here, #__VA_ARGS__)
#	define alert(...) if (bool(__VA_ARGS__)) breakpoint(), sys::err(here, #__VA_ARGS__)
#	define trace(...) sys::warn(here, #__VA_ARGS__)
#	define verify(...) assert(__VA_ARGS__)
#else
#	define breakpoint()
#	define assert(...)
#	define alert(...) (__VA_ARGS__)
#	define trace(...)
#	define verify(...) (__VA_ARGS__)
#endif

// Pessimistic boolean

enum : bool { success = false, failure = true };

static_assert(failure != success);

// Error format

namespace fmt
{
	struct where { const char* file; int line; const char* func; };

	template <typename... T> auto err(where at, T&&... t)
	{
		string::stream ss;
		ss << at.file << "(" << at.line << ")" << at.func << ":";
		if constexpr (0 < sizeof...(T))
		{
			((ss << ' ' << t), ...);
		}
		return ss.str();
	}
}

// Error pipe

namespace sys
{
	fmt::string::out::ref out(); // Output device
	fmt::string::out::ref flush(fmt::string::out::ref);

	namespace impl
	{
		int bug(fmt::string::view, bool);
	}

	extern bool debug; // Whether to write out

	template <typename... T> int warn(fmt::where at, T... t)
	{
		return debug ? impl::bug(fmt::err(at, t...), false) : -1;
	}

	template <typename... T> int err(fmt::where at, T... t)
	{
		return debug ? impl::bug(fmt::err(at, t...), true) : -1;
	}
}

// Unit tests

#ifndef NDEBUG
#	include "test.hpp"
#endif

#endif // file
