#include "test.hpp"
#include "fmt.hpp"
#include <stdexcept>

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

namespace fmt
{

TEST(fmt_empty,
{
	ASSERT(empty(std::string { }));
	ASSERT(not empty(string { HELLO_WORLD }));
	ASSERT(empty(string_view { }));
	ASSERT(not empty(string_view { HELLO_WORLD }));
	ASSERT(empty(span_view { }));
	ASSERT(not empty(span_view { HELLO_WORLD }));
	ASSERT(empty(span_view { string_view { } }));
});

TEST(fmt_terminated,
{
	ASSERT(terminated(string_view(HELLO_WORLD, 13)));
	ASSERT(not terminated(string_view(HELLO_WORLD, 5)));
});

TEST(fmt_trim,
{
	string whitespace = " \t\n";
	ASSERT(not trim(whitespace));
	constexpr auto raw = " \t" HELLO_WORLD "\n";
	string s = raw;
	ASSERT(trim(s));
	ASSERT_EQ(s, HELLO_WORLD);
	ASSERT_NOT_EQ(s, raw);
});

TEST(fmt_wide,
{
	ASSERT_EQ(to_wstring(string { HELLO_WORLD }), HELLO_WIDE);
	ASSERT_EQ(to_string(wstring { HELLO_WIDE }), HELLO_WORLD);
});

TEST(fmt_case,
{
	ASSERT_EQ(to_upper(HELLO_WORLD), HELLO_UPPER);
	ASSERT_EQ(to_lower(HELLO_WORLD), HELLO_LOWER);
});

//TEST(fmt_join, ASSERT_EQ(join({ "Hello,", "World!" }, " "), HELLO_WORLD));

//TEST(fmt_split, ASSERT_EQ((split(HELLO_WORLD, " "), span_view { "Hello,", "World!" })));

//TEST(fmt_replace, ASSERT_EQ((replace("Hello, user!", "user", "World"), HELLO_WORLD)));

} // fmt

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
