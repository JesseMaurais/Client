#ifndef test_hpp
#define test_hpp

#include <cassert>

#define ASSTRING(s) #s
#define STRING(n) ASSTRING(n)

#ifndef NDEBUG
namespace debug
{
	int run(int argc, char **argv);

	struct test
	{
		virtual void run() = 0;
		test(char const *name);
		~test();
	};
}
#	define TEST(name, ...) static struct : debug::test { void run() final { __VA_ARGS__; } using test::test; } name(#name)
#	define ASSERT(condition) if (not(condition)) throw __FILE__ ":" STRING(__LINE__) ": " #condition
#	define VERIFY(condition) ASSERT(condition)
#	define verify(condition) assert(condition)
#else
#	define TEST(name, ...)
#	define ASSERT(condition)
#	define VERIFY(condition) (condition)
#	define verify(condition) (condition)
#endif

#define ASSERT_EQ(a, b) ASSERT((a) == (b))
#define ASSERT_NOT_EQ(a, b) ASSERT((a) != (b))

#endif // file
