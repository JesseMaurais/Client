#include "fmt.hpp"
#include "err.hpp"

#define HELLO_WORLD "Hello, World!"
#define HELLO_WIDE L"Hello, World!"
#define HELLO_UPPER "HELLO, WORLD!"
#define HELLO_LOWER "hello, world!"
#define DLROW_OLLEH "!dlroW ,olleH"

#ifndef NDEBUG
#include "bug.hpp"
void test::entity<test::unit::fmt>::run() noexcept
{
	constexpr auto whitespace = " \t\n";
	fmt::string::view const Hello = "Hello, World!";

	assert(fmt::terminated(Hello));
	assert(not fmt::terminated(Hello.substr(0, 5)));
	assert(
}
#endif

