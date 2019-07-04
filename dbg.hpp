#ifndef dbg_hpp
#define dbg_hpp

#include <cassert>

#define ASSTRING(s) #s
#define STRING(n) ASSTRING(n)

#ifndef NDEBUG
namespace debug
{
	int run(char const *pattern);

	struct test
	{
		virtual void run() = 0;
		test(char const *name);
		~test();
	};
}
constexpr bool DEBUG = true;
#define TEST(unit, ...) struct : debug::test { void run() final { __VA_ARGS__; } using test::test; } unit(#unit);
#define ASSERT(...) if (not(__VA_ARGS__)) throw __FILE__ ":" STRING(__LINE__) ": " #__VA_ARGS__
#define verify(...) assert(__VA_ARGS__)
#else
constexpr bool DEBUG = false;
#define TEST(unit, ...) 
#define ASSERT(...)
#define verify(...) (__VA_ARGS__)
#endif

#define ASSERT_EQ(a, b) ASSERT((a) == (b))
#define ASSERT_NOT_EQ(a, b) ASSERT((a) != (b))
#define TEST_FAIL(unit, ...) TEST(unit, try { __VA_ARGS__; } catch (...) { return; } throw "Did not throw";)

#undef assert
#define assert(...) ASSERT(__VA_ARGS__)

#endif // file
