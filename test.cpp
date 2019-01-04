#include "dbg.hpp"
#include "fmt.hpp"
#include "ios.hpp"
#include "xdg.hpp"
#include "sys.hpp"
#include <fstream>
#include <sstream>

static_assert(DEBUG, "Compiling unit tests without debug mode.");

int main(int argc, char **argv)
{
	return debug::run(argc, argv);
}

//
// Sanity test the unit test code itself
//

namespace dbg
{
	TEST(sane, ASSERT(true and not false));
	TEST(sane_equality, ASSERT_EQ(true, true));
	TEST(sane_inequality, ASSERT_NOT_EQ(true, false));
}

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

} // fmt

//
// Operating system environment
//
/*
namespace env
{
	TEST(env_variables,
	{
		auto print = [](std::string_view u, std::string_view v) -> std::string
		{
			return empty(v) ? to_string(u) : fmt::key_value(u, v);
		};

		std::ofstream f { "sys.ini" };
		f << "[Environment Entry]" << std::endl;
		f << print("HOME", env::home) << std::endl;
		f << print("USER", env::user) << std::endl;
		f << print("PWD", env::pwd) << std::endl;
		f << print("LANG", env::lang) << std::endl;
		f << print("SHELL", env::shell) << std::endl;
		f << print("TMPDIR", env::tmpdir) << std::endl;
		f << print("DESKTOP", env::desktop) << std::endl;
		f << print("PROMPT", env::prompt) << std::endl;
		f << "[Freedesktop Entry]" << std::endl;
		f << print("XDG_CURRENT_DESKTOP", xdg::current_desktop) << std::endl;
		f << print("XDG_MENU_PREFIX", xdg::menu_prefix) << std::endl;
		f << print("XDG_RUNDTIME_DIR", xdg::runtime_dir) << std::endl;
		f << print("XDG_DATA_HOME", xdg::data_home) << std::endl;
		f << print("XDG_CONFIG_HOME", xdg::config_home) << std::endl;
		f << print("XDG_CACHE_HOME", xdg::cache_home) << std::endl;
		//f << print("XDG_DATA_DIRS", fmt::join(xdg::data_dirs, sys::sep::path));
		//f << print("XDG_CONFIG_DIRS", fmt::join(xdg::config_dirs, sys::sep::path));
	});
}
*/
//
// ANSI escape sequence
//

namespace io
{
	TEST(ansi_params,
	{
		std::ostringstream ss;
		ss << params<1, 2, 3, 4>;
		std::string const s = ss.str();
		ASSERT_EQ(s, "1;2;3;4");
	});

	TEST(ansi_fg, 
	{
		std::ostringstream ss;
		ss << fg_green << "GREEN" << fg_off;
		std::string const s = ss.str();
		ASSERT_EQ(s, "\x1b[32mGREEN\x1b[39m");
	});

} //sh

//
// Inter-process communications
//

namespace ipc
{
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
}

