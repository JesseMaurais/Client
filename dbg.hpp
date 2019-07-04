#ifndef dbg_hpp
#define dbg_hpp

#include <cassert>

#define ASSTRING(s) #s
#define STRING(n) ASSTRING(n)

#ifndef NDEBUG
namespace dbg
{
	int run(char const *pattern);

	struct test
	{
		virtual void run() = 0;
		test(char const *name);
		~test();
	};

	struct fail : test
	{
		using test::test;
		void run() final;
		virtual void run2() = 0;
	};
}
constexpr bool DEBUG = true;
#define TEST(unit) struct unit : dbg::test { using test::test; void run() final; } test_##unit(#unit); void unit::run()
#define FAIL(unit) struct unit : dbg::fail { using fail::fail; void run2() final; } test_##unit(#unit); void unit::run2()
#define ASSERT(...) if (not(__VA_ARGS__)) throw __FILE__ ":" STRING(__LINE__) ": " #__VA_ARGS__
#define verify(...) assert(__VA_ARGS__)
#else
constexpr bool DEBUG = false;
#define TEST(unit) (void) []()
#define ASSERT(...)
#define verify(...) (void) (__VA_ARGS__)
#endif

#define ASSERT_EQ(a, b) ASSERT((a) == (b))
#define ASSERT_NOT_EQ(a, b) ASSERT((a) != (b))
#undef assert
#define assert(...) ASSERT(__VA_ARGS__)

#endif // file
