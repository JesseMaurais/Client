// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dbg.hpp"
#include "alg.hpp"
#include "int.hpp"
#include "fmt.hpp"
#include "ios.hpp"
#include "xdg.hpp"
#include "sys.hpp"
#include "net.hpp"
#include "sig.hpp"
#include "ipc.hpp"
#include "mem.hpp"
#include "file.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <csignal>

static_assert(DEBUG, "Compiling unit tests without debug mode.");

int main(int argc, char **argv)
{
	(void) argc;
	return debug::run(argv[1]);
}

// Some strings to use in tests

#define HELLO_WORLD "Hello, World!"
#define HELLO_WIDE L"Hello, World!"
#define HELLO_UPPER "HELLO, WORLD!"
#define HELLO_LOWER "hello, world!"
#define DLROW_OLLEH "!dlroW ,olleH"

//
// Sanity test the unit test code itself
//

namespace
{
	//TEST(sane, ASSERT(true and not false));
	//TEST(sane_equality, ASSERT_EQ(true, true));
	//TEST(sane_inequality, ASSERT_NOT_EQ(true, false));
	//TEST_THROW(sane_negative, (void) 0);
}

//
// Checked integer conversions
//

namespace
{
	TEST(int_narrow, ASSERT_EQ('*', to_narrow<char>(42)));
	TEST(int_unsigned, ASSERT_EQ(5u, to_unsigned(5)));
	TEST(int_signed, ASSERT_EQ(42, to_signed(42u)));

	// negative tests

	TEST_THROW(int_loss, (void) to_narrow<char>(257));
	TEST_THROW(int_sign, (void) to_unsigned(-1));
}

//
// Base template algorithms
//

namespace
{
	TEST(alg_sort,
	{
		using namespace stl;

		std::vector num { 5, 7, 3, 9, 6, 1, 2, 0, 4, 8 };
		sort(num, [](int a, int b) { return a < b; });
		ASSERT_EQ(num, (std::vector { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }));
	});

	TEST(alg_find,
	{
		using namespace stl;

		std::vector const pow2 { 2, 4, 8, 16, 32, 64, 128, 256 };
		ASSERT_EQ(find(pow2, 13), end(pow2));
		ASSERT_NOT_EQ(find(pow2, 64), end(pow2));

		auto const is_even = [](int n) { return (n % 2) == 0; };
		ASSERT(all_of(pow2, is_even));

		std::vector<int> odds;
		for_each(pow2, [&odds](int n) { odds.push_back(n + 1); }); 
		ASSERT(none_of(odds, is_even));
	});

	TEST(alg_copy,
	{
		using namespace stl;

		std::vector<int> u(9);
		generate(u, [n = 0]() mutable { return ++n; });
		ASSERT_EQ(u, (std::vector { 1, 2, 3, 4, 5, 6, 7, 8, 9 }));

		std::vector<int> v(9);
		copy(u, begin(v));
		ASSERT_EQ(u, v);

		erase_if(u, [](int n) { return n % 2; });
		ASSERT_EQ(u, (std::vector { 2, 4, 6, 8 }));
	});
}

//
// Text formatting routines
//

namespace
{

	TEST(fmt_empty,
	{
		using namespace fmt;
		ASSERT(empty(string { }));
		ASSERT(not empty(string { HELLO_WORLD }));
		ASSERT(empty(string_view { }));
		ASSERT(not empty(string_view { HELLO_WORLD }));
		ASSERT(empty(span_view { }));
		ASSERT(not empty(span_view { HELLO_WORLD }));
		ASSERT(empty(span_view { string_view { } }));
	});

	TEST(fmt_terminated,
	{
		using namespace fmt;
		ASSERT(terminated(string_view(HELLO_WORLD, 13)));
		ASSERT(not terminated(string_view(HELLO_WORLD, 5)));
	});

	TEST(fmt_trim,
	{
		using namespace fmt;
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
		using namespace fmt;
		ASSERT_EQ(to_wstring(string { HELLO_WORLD }), HELLO_WIDE);
		ASSERT_EQ(to_string(wstring { HELLO_WIDE }), HELLO_WORLD);
	});

	TEST(fmt_case,
	{
		using namespace fmt;
		ASSERT_EQ(to_upper(HELLO_WORLD), HELLO_UPPER);
		ASSERT_EQ(to_lower(HELLO_WORLD), HELLO_LOWER);
	});
}

//
// Operating system environment
//

namespace
{
	TEST(env_eval,
	{
		std::string const var = fmt::join({sys::esc::sh::first, "PATH", sys::esc::sh::second});
		std::string const val = fmt::join(env::path, sys::sep::path);
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
		f << fmt::key_value("ROOT", env::root) << std::endl;
		f << fmt::key_value("PWD", env::pwd) << std::endl;
		f << fmt::key_value("LANG", env::lang) << std::endl;
		f << fmt::key_value("SHELL", env::shell) << std::endl;
		f << fmt::key_value("TMPDIR", env::tmpdir) << std::endl;
		f << fmt::key_value("ROOTDIR", env::rootdir) << std::endl;
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
// Signal handlers
//

namespace
{
	TEST(sig_handler,
	{
		std::vector<int> caught;
		for (int const n : { SIGINT, SIGFPE, SIGILL })
		{
			sys::sig::slot handle(n, [&](int signo)
			{
				caught.push_back(signo);
			});
			std::raise(n);
		}

		ASSERT_NOT_EQ(stl::find(caught, SIGINT), caught.end());
		ASSERT_NOT_EQ(stl::find(caught, SIGFPE), caught.end());
		ASSERT_NOT_EQ(stl::find(caught, SIGILL), caught.end());

		ASSERT_EQ(stl::find(caught, SIGSEGV), caught.end());
		ASSERT_EQ(stl::find(caught, SIGTERM), caught.end());
		ASSERT_EQ(stl::find(caught, SIGABRT), caught.end());
	});
}

//
// ANSI escape sequence
//

namespace
{
	TEST(ansi_params,
	{
		std::ostringstream ss;
		ss << io::params<1, 2, 3, 4>;
		std::string const s = ss.str();
		ASSERT_EQ(s, "1;2;3;4");
	});

	TEST(ansi_fg, 
	{
		std::ostringstream ss;
		ss << io::fg_green << "GREEN" << io::fg_off;
		std::string const s = ss.str();
		ASSERT_EQ(s, "\x1b[32mGREEN\x1b[39m");
	});

}

//
// Inter-process communications
//

namespace
{
	TEST(ipc_rev,
	{
		sys::io::pstream ps { "rev" };
		ps << HELLO_WORLD;
		ps.close(0);
		std::string s;
		ASSERT(std::getline(ps, s));
		ASSERT_EQ(s, DLROW_OLLEH);
	});

	TEST(ipc_mem,
	{
		sys::file::descriptor file;
		file.open(__FILE__, sys::file::in);
		sys::file::view map(file.get());

		fmt::string_view const view = map;
		auto pos = view.find("Self referencing find.");
		ASSERT_NOT_EQ(pos, fmt::string_view::npos);
	});
}

