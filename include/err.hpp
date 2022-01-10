#ifndef err_hpp
#define err_hpp "Error Format"

#include "fmt.hpp"

#ifdef assert
#	undef assert
#	warning You should not include assert
#endif

#ifndef NDEBUG
#	define assert(...) if (not(__VA_ARGS__)) sys::warn(here, #__VA_ARGS__)
#	define alert(...) if (bool(__VA_ARGS__)) sys::err(here, #__VA_ARGS__)
#	define trace(...) sys::warn(here, __VA_ARGS__)
#	define verify(...) assert(__VA_ARGS__)
#else
#	define assert(...)
#	define alert(...) (__VA_ARGS__)
#	define trace(...)
#	define verify(...) (__VA_ARGS__)
#endif

#ifndef STRINGIFY
#	define STRINGIFY(x) #x
#endif

#ifdef _WIN32
#	define FILELINE __FILE__ "(" STRINGIFY(__LINE__) ")"
#else
#	define FILELINE __FILE__ ":" STRINGIFY(__LINE__)
#endif

#define here fmt::where { __FILE__, __LINE__, __func__ }

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

namespace sys
{
	extern bool debug; // whether to write out errors
	int perror(fmt::string::view, bool perr = true); // print out
	fmt::string::out::ref out(); // thread-safe buffered output device
	fmt::string::out::ref put(fmt::string::out::ref); // flush out error

	template <typename... T> int warn(fmt::where at, T... t)
	{
		return debug ? perror(fmt::err(at, t...), false) : -1;
	}

	template <typename... T> int err(fmt::where at, T... t)
	{
		return debug ? perror(fmt::err(at, t...), true) : -1;
	}
}

enum : bool { success = false, failure = true };

#ifndef NDEBUG
#	include "test.hpp"
#endif

#endif // file
