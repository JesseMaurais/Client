#ifndef err_hpp
#define err_hpp "Error Format"

#include "fmt.hpp"
#include "sym.hpp"
#include <sstream>

#ifdef assert
#	undef assert
#	warning You should not include assert
#endif

#ifndef NDEBUG
#	define assert(...) if (not(__VA_ARGS__)) sys::warn(here, #__VA_ARGS__)
#	define alert(...) if (bool(__VA_ARGS__)) sys::err(here, #__VA_ARGS__)
#	define verify(...) assert(__VA_ARGS__)
#else
#	define assert(...)
#	define alert(...) (__VA_ARGS__)
#	define verify(...) (__VA_ARGS__)
#endif

#ifndef NDEBUG
#	define test_unit(name) \
		dynamic void test_##name([[maybe_unused]] fmt::span args)
#	define except(expression) \
		try { (void)(expression); assert(not #expression); } catch(...) { }
#endif

#define here fmt::where { __FILE__, __LINE__, __func__ }

enum : bool { success = false, failure = true };

namespace fmt
{
	struct where { view file; int line; view func; };

	template <typename... T> auto error(where at, T&&... va)
	{
		std::stringstream ss;
		ss << at.file << "(" << at.line << ")" << at.func << ":";
		if constexpr (0 < sizeof...(T)) ((ss << ' ' << va), ...);
		return ss.str();
	}
}

namespace sys
{
	extern bool debug; // whether to write out errors
	int perror(fmt::view, bool errn = true); // print out

	fmt::output out(); // thread-local buffered output device
	fmt::output put(fmt::output); // flush out error

	template <typename... T> int warn(fmt::where at, T... va)
	{
		return debug ? perror(fmt::error(at, va...), false) : -1;
	}

	template <typename... T> int err(fmt::where at, T... va)
	{
		return debug ? perror(fmt::error(at, va...), true) : -1;
	}
}

#endif // file
