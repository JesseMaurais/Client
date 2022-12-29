#ifndef err_hpp
#define err_hpp "Error Format"

#include <cstring>
#include <ostream>

#ifdef assert
#	undef assert
#	warning You should not include assert. Force __FILE__ to be included first.
#else // redefine using standard access
#	include <assert.h>
#	undef assert
#endif

#define HERE(X) ::fmt::where { __FILE__, __func__, __LINE__, #X }

#ifndef NDEBUG
#	define assert(X) if (not(X))::fmt::warning(HERE(X))
#	define alert(X) if (int x=X)::fmt::error(x, HERE(X))
#	define perror(X) alert(errno) << X
#else
#	define assert(X)
#	define alert(X)
#	define perror(X)
#endif

namespace fmt
{
	struct where
	{
		const char * file;
		const char * func;
		unsigned int line;
		const char * code;
	};

	using os = std::ostream;

	os& out(); os& put(os&);

	template <class... Type> os& printer(os& out, where const& at, Type const&... args)
	{
		out << at.file << "(" << at.line << ")" << at.func << ":" << at.code << ";";
		if constexpr (0 < sizeof...(Type)) ((out << ' ' << args), ...);
		return out;
	}

	extern bool debug;

	template <class... Type> os& warning(where const& at, Type const&... args)
	{
		return printer(out(), at, args...);
	}

	template <class... Type> os& error(int no, where const& at, Type const&... args)
	{
		return warning(at, std::strerror(no), args...);
	}
}

#include "sym.hpp" // dynamic

#define STR(str) #str
#define CAT(left, right) left##right
#define UNIT(name) CAT(name, _UNIT_TEST_SUFFIX)
#define TEST(name) dynamic void UNIT(name) ()

namespace env::test
{
	constexpr auto suffix = STR(_UNIT_TEST_SUFFIX);
};

#endif // file
