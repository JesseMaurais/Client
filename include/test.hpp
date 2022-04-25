#ifndef test_hpp
#define test_hpp "Unit Tests"

#include "err.hpp"
#include "sym.hpp"

#ifdef NDEBUG
#	warning You should only compile unit tests in debug
#endif

// Assert just in case expression did not throw
#define except(expression)\
	try { (void)(expression); assert(not #expression); } catch(...) { }

// Supply the signature for a unit test callback
#define test_unit(name) dynamic void test_##name()

#endif // file
