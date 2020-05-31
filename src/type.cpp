#include "type.hpp"
#include "esc.hpp"
#include "err.hpp"

namespace
{
	fmt::type<char> const CSTR;
	fmt::type<wchar_t> const WSTR;
}

namespace fmt
{
	type<char> const &cstr = CSTR;
	type<wchar_t> const &wstr = WSTR;
}

#ifdef test
test(type)
{
	using namespace fmt;

	string::view const Space = " \t\n";
	assert(empty(trim(Space)));

	string::view const Hello = "Hello, World!";
	assert(terminated(Hello));
	assert(not terminated(Hello.substr(0, 5)));

	string const Filled = to_string(Space) + to_string(Hello) + to_string(Space);
	assert(not empty(trim(Filled)));
	assert(Hello == trim(Filled));

	string::view const Upper = "HELLO, WORLD!";
	assert(to_upper(Hello) == Upper);

	string::view const Lower = "hello, world!";
	assert(to_lower(Hello) == Lower);

	wstring::view const Wide = L"Hello, World!";
	assert(to_wstring(Hello) == Wide);
	assert(to_string(Wide) == Hello);

	using pos = std::pair<string::size_type, string::size_type>;

	assert(embrace("<A<B>C>", "<>") == pos { 0, 6 });
	assert(embrace("<A<B>C>", "<B>") == pos { 0, 3 });
	assert(embrace("A[B]C[D]", "[D]") == pos { 1, 3 });
	assert(embrace("{A<B}C}", "<>") == pos { 2, npos });
	assert(embrace("{A{B>C}", "<>") == pos { npos, npos });
	assert(embrace("&amp;", "&;") == pos { 0, 4 });

	{
		string::stream ss;
		ss << par<1, 2, 3, 4>;
		assert(ss.str() == "1;2;3;4");
	}

	{
		string::stream ss;
		ss << fg_green << "GREEN" << fg_off;
		assert(ss.str() == "\x1b[32mGREEN\x1b[39m");
	}
}
#endif

