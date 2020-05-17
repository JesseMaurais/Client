#ifndef err_hpp
#define err_hpp "Error Format"

#include <system_error>
#include <exception>
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
#	define trace(...) sys::warn(where, __VA_ARGS__)
#	define verify(x) assert(x)
#else
#	define assert(x)
#	define alert(x) 
#	define trace(...)
#	define verify(x) (x)
#endif

// if only(failure) out << "Only without NDEBUG";

// Negative boolean

enum : bool { success = false, failure = true };

static_assert(failure != success);

// Error format

namespace fmt
{
	struct where { const char* file; int line; const char* func; };

	string::out::ref operator<<(string::out::ref, std::errc const &);
	string::out::ref operator<<(string::out::ref, std::exception const &);
	string::out::ref operator<<(string::out::ref, where);

	template <typename T, typename... S> auto err(T t, S... s)
	{
		string::stream ss;
		ss << t;
		if constexpr (0 < sizeof...(s)) ((ss << ' ' << s), ...);
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

	extern bool quiet; // Whether to warn on err

	template <typename... T> int warn(T... t)
	{
		return quiet ? -1 : impl::bug(fmt::err(t...), false);
	}

	template <typename... T> int err(T... t)
	{
		return quiet ? -1 : impl::bug(fmt::err(t...), true);
	}
}

#ifndef NDEBUG
# include "bug.hpp"
#endif

#endif // file
