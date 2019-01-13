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
#define TEST(unit, ...) static struct : debug::test { void run() final { __VA_ARGS__; } using test::test; } unit(#unit)
#define ASSERT(...) if (not(__VA_ARGS__)) throw __FILE__ ":" STRING(__LINE__) ": " #__VA_ARGS__
#define VERIFY(...) ASSERT(__VA_ARGS__)
#define verify(...) assert(__VA_ARGS__)
#else
constexpr bool DEBUG = false;
#define TEST(name, ...)
#define ASSERT(...)
#define VERIFY(...) (__VA_ARGS__)
#define verify(...) (__VA_ARGS__)
#endif

#define ASSERT_EQ(a, b) ASSERT((a) == (b))
#define ASSERT_NOT_EQ(a, b) ASSERT((a) != (b))

#endif // file
