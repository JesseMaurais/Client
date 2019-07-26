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
		virtual void die() = 0;
	};
}
#define TEST(unit) struct unit : dbg::test { using test::test; void run() final; } test_##unit(#unit); void unit::run()
#define FAIL(unit) struct unit : dbg::fail { using fail::fail; void die() final; } test_##unit(#unit); void unit::die()
#else
#define TEST(unit) (void) []()
#define FAIL(unit) (void) []()
#endif

#endif // file
