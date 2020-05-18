#ifndef bug_hpp
#define bug_hpp "Debug Tests"

#include "sym.hpp"

#ifdef NDEBUG
#	warning You should compile unit tests in debug
#endif

// assert just in case expression did not throw
#define except(expression)\
	try { (void)(expression); assert(not #expression); }\
	catch(...) { }

// supply the signature for a unit test callback
#define test(name) dynamic void test_##name()

#endif // file
