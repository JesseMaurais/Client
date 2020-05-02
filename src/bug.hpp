#ifndef bug_hpp
#define bug_hpp "Debug Tests"

#ifndef NDEBUG
# warning You should compile unit tests in debug
#endif

// assert just in case expression did not throw
#define except(expression)\
	try { (void) expression; assert(not #expression); } catch(...) { }

namespace test
{
	enum class unit
	{
		bug, fmt, dig, env, dir, usr, ini, sig, dll
	};

	template <unit Test> void run() noexcept;
}

#endif // file
