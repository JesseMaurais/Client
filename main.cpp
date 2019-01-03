#include "dbg.hpp"
#include "fmt.hpp"
#include "ios.hpp"
#include "xdg.hpp"
#include "sys.hpp"
#include <fstream>

static_assert(DEBUG, "Compiling unit tests with debugging off.");

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

namespace env
{
	TEST(env_environment,
	{
		auto print = [](std::string_view u, std::string_view v)
		{
			return fmt::key_value(u, v) + '\n';
		};

		std::ofstream f { "sys.ini" };
		f << "[Environment Entry]" << std::endl;
		f << print("HOME", env::home);
		f << print("USER", env::user);
		f << print("PWD", env::pwd);
		f << print("LANG", env::lang);
		f << print("SHELL", env::shell);
		f << print("TMPDIR", env::tmpdir);
		f << print("DESKTOP", env::desktop);
		f << print("PROMPT", env::prompt);
		f << "[Freedesktop Entry]" << std::endl;
		f << print("XDG_CURRENT_DESKTOP", xdg::current_desktop);
		f << print("XDG_MENU_PREFIX", xdg::menu_prefix);
		f << print("XDG_RUNDTIME_DIR", xdg::runtime_dir);
		f << print("XDG_DATA_HOME", xdg::data_home);
		f << print("XDG_CONFIG_HOME", xdg::config_home);
		f << print("XDG_CACHE_HOME", xdg::cache_home);
		//f << print("XDG_DATA_DIRS", fmt::join(xdg::data_dirs, sys::sep::path));
		//f << print("XDG_CONFIG_DIRS", fmt::join(xdg::config_dirs, sys::sep::path));
		f << print("XDG_DESKTOP_DIR", xdg::desktop_dir);
		f << print("XDG_DOCUMENTS_DIR", xdg::documents_dir);
		f << print("XDG_DOWNLOAD_DIR", xdg::download_dir);
		f << print("XDG_MUSIC_DIR", xdg::music_dir);
		f << print("XDG_PICTURES_DIR", xdg::pictures_dir);
		f << print("XDG_PUBLICSHARE_DIR", xdg::publicshare_dir);
		f << print("XDG_TEMPLATES_DIR", xdg::templates_dir);
		f << print("XDG_VIDEOS_DIR", xdg::videos_dir);
	});
}

//
// ANSI escape sequence
//

#include <sstream>

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

