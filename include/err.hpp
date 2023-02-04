#ifndef err_hpp
#define err_hpp "Error Format"

#include <cstring>
#include <ostream>

#ifdef assert
#	undef assert
#	warning You should not include assert. Include __FILE__ instead.
#else
#	include <cassert>
#	undef assert
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

	std::ostream& out(); std::ostream& put(std::ostream&);

	template <class... Type> auto& printer(std::ostream& out, const where& at, const Type&... args)
	{
		out << at.file << "(" << at.line << ")" << at.func << ":" << at.code << ";";
		if constexpr (0 < sizeof...(Type)) ((out << ' ' << args), ...);
		return out;
	}

	extern bool debug;

	template <class... Type> auto& warn(const where& at, const Type&... args)
	{
		return printer(out(), at, args...);
	}

	template <class... Type> auto& err(int no, const where& at, const Type&... args)
	{
		return warn(at, std::strerror(no), args...);
	}
}

#ifndef NDEBUG
#	define HERE(X) ::fmt::where { __FILE__, __func__, __LINE__, X }
#	define ERR(n, X, ...) ::fmt::err(n, HERE(X), ## __VA_ARGS__);
#	define WARN(X, ...) ::fmt::warn(HERE(X), ## __VA_ARGS__)
#	define ALERT(X, ...) if (int n_=(X)) ERR(n_, #X, ## __VA_ARGS__)
#	define ASSERT(X, ...) if (not(X)) WARN(#X, ## __VA_ARGS__)
#	define EXCEPT(X, ...) try { (X); ASSERT(!#X, ## __VA_ARGS__); } catch (...) { }
#endif

#define assert(X, ...) ASSERT(X, ## __VA_ARGS__)
#define perror(...) ALERT(errno, ## __VA_ARGS__)
#define except(X, ...) EXCEPT(X, ## __VA_ARGS__)

#include "sym.hpp" // dynamic

#define TEST(name) dynamic void name##_UNIT_TEST_SUFFIX ()

namespace env::test
{
	constexpr auto suffix = "_UNIT_TEST_SUFFIX";
};

#endif // file
