#ifndef bug_hpp
#define bug_hpp "Debug Tests"

#include "dll.hpp"

#ifndef NDEBUG
# warning You should compile unit tests in debug
#endif

// assert just in case expression did not throw
#define except(expression)\
	try { (void) expression; assert(not #expression); } catch(...) { }

#define TEST(name) dynamic test_##name() noexcept 

#endif // file
