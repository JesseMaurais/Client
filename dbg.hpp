#ifndef dbg_hpp
#define dbg_hpp

#include "err.hpp"

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
#define TEST(unit) struct unit : dbg::test { using test::test; void run() final; } test_##unit(#unit); void unit::run()
#define FAIL(unit) struct unit : dbg::fail { using fail::fail; void run2() final; } test_##unit(#unit); void unit::run2()
#define ASSERT(x) { if (not(x)) throw ::fmt::err(here, #x); }
#define VERIFY(x) ASSERT(x)
#else
#define TEST(unit) (void) []()
#define FAIL(unit) (void) []()
#define ASSERT(x)
#define VERIFY(x) (x)
#endif

#define ASSERT_EQ(a, b) ASSERT((a) == (b))
#define ASSERT_NOT_EQ(a, b) ASSERT((a) != (b))

#endif // file
