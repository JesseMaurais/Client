// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dbg.hpp"
#include "fmt.hpp"
#include "ios.hpp"
#include "xdg.hpp"
#include "sys.hpp"
#include "file.hpp"
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
}

//
// Operating system environment
//

namespace env
{
	TEST(env_eval,
	{
		std::string const var = fmt::join({sys::esc::sh::first, "PATH", sys::esc::sh::second});
		std::string const val = fmt::join(path, sys::sep::path);
		ASSERT_EQ(sys::env::eval(var), val);
	});

	static std::ofstream& outfile()
	{
		static std::ofstream f { "sys.ini" };
		return f;
	}

	TEST(env_vars,
	{
		auto& f = outfile();
		f << "[Environment]" << std::endl;
		for (auto const v : (fmt::span_view) env::variables)
		{
			f << v << std::endl;
		}
	});

	TEST(env_fake,
	{
		auto& f = outfile();
		f << "[Fake Environment]" << std::endl;
		f << fmt::key_value("HOME", env::home) << std::endl;
		f << fmt::key_value("USER", env::user) << std::endl;
		f << fmt::key_value("PWD", env::pwd) << std::endl;
		f << fmt::key_value("LANG", env::lang) << std::endl;
		f << fmt::key_value("SHELL", env::shell) << std::endl;
		f << fmt::key_value("TMPDIR", env::tmpdir) << std::endl;
		f << fmt::key_value("DESKTOP", env::desktop) << std::endl;
		f << fmt::key_value("PROMPT", env::prompt) << std::endl;
	});

	TEST(xdg_desktop,
	{
		auto& f = outfile();
		f << "[Desktop]" << std::endl;
		f << fmt::key_value("XDG_CURRENT_DESKTOP", xdg::current_desktop) << std::endl;
		f << fmt::key_value("XDG_MENU_PREFIX", xdg::menu_prefix) << std::endl;
		f << fmt::key_value("XDG_APPLICATIONS_MENU", xdg::applications_menu) << std::endl;
	});

	TEST(xdg_data,
	{
		auto& f = outfile();
		f << "[Data Directories]" << std::endl;
		f << fmt::key_value("XDG_RUNDTIME_DIR", xdg::runtime_dir) << std::endl;
		f << fmt::key_value("XDG_DATA_HOME", xdg::data_home) << std::endl;
		f << fmt::key_value("XDG_CONFIG_HOME", xdg::config_home) << std::endl;
		f << fmt::key_value("XDG_CACHE_HOME", xdg::cache_home) << std::endl;
		f << fmt::key_value("XDG_DATA_DIRS", fmt::join(xdg::data_dirs, sys::sep::path)) << std::endl;
		f << fmt::key_value("XDG_CONFIG_DIRS", fmt::join(xdg::config_dirs, sys::sep::path)) << std::endl;
	});

	TEST(xdg_user,
	{
		auto& f = outfile();
		f << "[User Directories]" << std::endl;
		f << fmt::key_value("XDG_DESKTOP_DIR", xdg::desktop_dir) << std::endl;
		f << fmt::key_value("XDG_DOCUMENTS_DIR", xdg::documents_dir) << std::endl;
		f << fmt::key_value("XDG_DOWNLOAD_DIR", xdg::download_dir) << std::endl;
		f << fmt::key_value("XDG_MUSIC_DIR", xdg::music_dir) << std::endl;
		f << fmt::key_value("XDG_PICTURES_DIR", xdg::pictures_dir) << std::endl;
		f << fmt::key_value("XDG_PUBLICSHARE_DIR", xdg::publicshare_dir) << std::endl;
		f << fmt::key_value("XDG_TEMPLATES_DIR", xdg::templates_dir) << std::endl;
		f << fmt::key_value("XDG_VIDEOS_DIR", xdg::videos_dir) << std::endl;
	});
}

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

}

//
// Inter-process communications
//

namespace ipc
{

}

