#include "test.hpp"
#include "fmt.hpp"
#include <iostream>

int main(int argc, char **argv)
{
	return debug::run(argc, argv);
}

//
// Sanity test the unit test code itself
//

TEST(sane, ASSERT(true and not false));
TEST(sane_equality, ASSERT_EQ(true, true));
TEST(sane_inequality, ASSERT_NOT_EQ(true, false));

//
// Text formatting routines
//

#define HELLO_WORLD "Hello, World!"
#define HELLO_WIDE L"Hello, World!"
#define HELLO_UPPER "HELLO, WORLD!"
#define HELLO_LOWER "hello, world!"

TEST(fmt_trim,
{
	constexpr auto raw = " \t" HELLO_WORLD "\n";
	std::string s = raw;
	ASSERT(fmt::trim(s));
	ASSERT_EQ(s, HELLO_WORLD);
	ASSERT_NOT_EQ(s, raw);
});

TEST(fmt_wide,
{
	constexpr auto S = HELLO_WORLD;
	constexpr auto W = HELLO_WIDE;
	ASSERT_EQ(fmt::to_wstring(S), W);
	ASSERT_EQ(fmt::to_string(W), S);
});

TEST(fmt_case,
{
	std::cout << fmt::to_upper(HELLO_WORLD) << std::endl;
	//ASSERT_EQ(fmt::to_upper(HELLO_WORLD), HELLO_UPPER);
	//ASSERT_EQ(fmt::to_lower(HELLO_WORLD), HELLO_LOWER);
});

//
// Inter-process communications
//

/*
TEST(ipc_pstream, 
{
	sys::io::pstream tr { "tr", "a-z", "A-Z" };
	tr << HELLO_WORLD;
	tr.close();
	std::string s;
	ASSERT(std::getline(tr, s));
	ASSERT_EQ(s, HELLO_UPPER);
});
*/
