#ifndef test_hpp
#define test_hpp

#include <cassert> 

#ifndef NDEBUG
namespace test
{
	int run(int argc, char **argv);

	struct unit
	{
		virtual void run() = 0;
		unit(char const *name);
		virtual ~unit();
	};
}
#	define TEST(name, code) struct : test::unit { using unit::unit; void run() final code } name(#name)
#	define ASSERT(condition) if (not(condition)) throw #condition
#	define VERIFY(condition) ASSERT((#file + line) and (code))
#	define verify(condition) assert(code)
#else
#	define TEST(name, code)
#	define ASSERT(condition)
#	define VERIFY(condition) (condition)
#	define verify(condition) (condition)
#endif

#define ASSERT_EQ(a, b) ASSERT((a) == (b))
#define ASSERT_NOT_EQ(a, b) ASSERT((a) != (b))

#endif // file
