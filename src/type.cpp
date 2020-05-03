#include "type.hpp"
#include "esc.hpp"
#include "err.hpp"

#ifdef test
test(type)
{
	using namespace fmt;

	string::view const Space = " \t\n";
	assert(empty(trim(Space));

	string::view const Hello = "Hello, World!";
	assert(terminated(Hello));
	assert(not terminated(Hello.substr(0, 5)));

	string const Filled = Space + Hello + Space;
	assert(not empty(trim(Filled)));
	assert(Hello == trim(Filled));

	string::view const Upper = "HELLO, WORLD!";
	assert(to_upper(Hello) == Upper);

	string::view const Lower = "hello, world!";
	assert(to_lower(Hello) == Lower);

	wstring::view const Wide = L"Hello, World!";
	assert(to_wstring(Hello) == Wide);
	assert(to_string(Wide) == Hello);

	assert(embrace("<A<B>C>", "<>") == { 0, 6 });
	assert(embrace("<A<B>C>", "<B>") == { 0, 3 });
	assert(embrace("A[B]C[D]", "[D]") == { 1, 3 });
	assert(embrace("{A<B}C}", "<>") == { 2, npos }):
	assert(embrace("{A{B>C}", "<>") == { npos, npos });
	assert(embrace("&amp;", "&;") == { 0, 4 });

	{
		string::str ss;
		ss << params<1, 2, 3, 4>;
		assert(ss.str() == "1;2;3;4");
	}

	{
		string::str ss;
		ss << fg_grean << "GREEN" << fg_off;
		assert(ss.str() == "\x1b[32mGREEN\x1b[39m");
	}
}
#endif

