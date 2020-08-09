// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

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
	{
		assert(terminated(Hello));
		auto substr = Hello.substr(0, 5);
		assert(not terminated(substr));
		auto termstr = string(substr.data(), substr.size());
		substr = termstr.c_str();
		assert(terminated(substr));
	}

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
		string::view::vector v  
		{ 
			"1 2 3", " 1 2 3", "1 2 3 ", " 1 2 3 ",
			"\t1\n2\n\t3\t\n"
		};
		for (auto u : v)
		{
			auto const t = split(u);
			assert(t.size() == 3);
			assert(t.front() == "1");
			assert(t.back() == "3");
		}
	}

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

