// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "type.hpp"
#include "char.hpp"
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

#ifdef test_unit
test_unit(type)
{
	fmt::string::view const Space = " \t\v\r\n\f";
	fmt::string::view const Hello = "Hello, World!";
	fmt::string::view const Upper = "HELLO, WORLD!";
	fmt::string::view const Lower = "hello, world!";
	fmt::wstring::view const Wide = L"Hello, World!";
	auto const Filled = fmt::to_string(Space) + fmt::to_string(Hello) + fmt::to_string(Space);

	// Character class/whitespace iteration
	{
		for (char c : Space)
		{
			assert(fmt::cstr.check(c));
		}
		assert(fmt::next(Space) == Space.begin());
		assert(fmt::skip(Space) == Space.end());
		assert('H' == *fmt::first(Filled));
		assert('!' == *fmt::last(Filled));
	}

	// Triming whitespace
	{
		assert(std::empty(fmt::trim(Space)));
		assert(not std::empty(fmt::trim(Filled)));
		assert(Hello == fmt::trim(Filled));
	}

	// Split by whitespace/characters
	{
		fmt::string::view::vector const v  
		{ 
			"1 2 3", " 1 2 3", "1 2 3 ", " 1 2 3 ",
			"\t1\n2\n\t3\t\n"
		};

		for (auto u : v)
		{
			auto const t = fmt::split(u);
			assert(t.size() == 3);
			assert(t.front() == "1");
			assert(t.back() == "3");
		}
	}

	// String view null terminator
	{
		assert(fmt::terminated(Hello));
		auto substr = Hello.substr(0, 5);
		assert(not fmt::terminated(substr));
	}

	// Character case conversion
	{
		assert(fmt::to_upper(Hello) == Upper);
		assert(fmt::to_lower(Hello) == Lower);
	}

	// Character encoding conversion
	{
		assert(fmt::to_wstring(Hello) == Wide);
		assert(fmt::to_string(Wide) == Hello);
	}

	// Search matching braces
	{
		using pos = std::pair<fmt::string::size_type, fmt::string::size_type>;

		assert(fmt::embrace("<A<B>C>", "<>") == pos { 0, 6 });
		assert(fmt::embrace("<A<B>C>", "<B>") == pos { 0, 3 });
		assert(fmt::embrace("A[B]C[D]", "[D]") == pos { 1, 3 });
		assert(fmt::embrace("{A<B}C}", "<>") == pos { 2, fmt::npos });
		assert(fmt::embrace("{A{B>C}", "<>") == pos { fmt::npos, fmt::npos });
		assert(fmt::embrace("&amp;", "&;") == pos { 0, 4 });
	}

	// Escape parameter encoding
	{
		fmt::string::stream ss;
		ss << fmt::io::par<1, 2, 3, 4>;
		assert(ss.str() == "1;2;3;4");
	}

	// Set graphics rendition
	{
		fmt::string::stream ss;
		ss << fmt::io::fg_green << "GREEN" << fmt::io::fg_off;
		assert(ss.str() == "\x1b[32mGREEN\x1b[39m");
	}
}
#endif

