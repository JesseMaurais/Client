// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dbg.hpp"
#include "int.hpp"
#include "fmt.hpp"
#include "del.hpp"
#include "sgr.hpp"
#include "xdg.hpp"
#include "sys.hpp"
#include "net.hpp"
#include "sig.hpp"
#include "dir.hpp"
#include "dll.hpp"
#include "mem.hpp"
#include "fifo.hpp"
#include "pstream.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <csignal>

#define HELLO_WORLD "Hello, World!"
#define HELLO_WIDE L"Hello, World!"
#define HELLO_UPPER "HELLO, WORLD!"
#define HELLO_LOWER "hello, world!"
#define DLROW_OLLEH "!dlroW ,olleH"

static_assert(DEBUG, "Compiling unit tests without debug mode.");

int main(int argc, char **argv)
{
	(void) argc;
	return dbg::run(argv[1]);
}

namespace
{
	//
	// Sanity test the unit test code itself
	// Disabled by default for code analyzers
	//
	
	//TEST(sane) { ASSERT(true and not false); }
	//TEST(sane_equality) { ASSERT_EQ(true, true); }
	//TEST(sane_inequality) { ASSERT_NOT_EQ(true, false); }
	//FAIL(sane_throw) { throw "sane"; }

	// negatives
	
	//TEST(not_sane) { ASSERT(false and not true); }
	//TEST(not_equal) { ASSERT_EQ(true, false); }
	//TEST(not_unequal) { ASSERT_NOT_EQ(true, true); }
	//FAIL(not_throw) { (void) 0; }
	
	//
	// Checked integer conversions
	//

	TEST(int_narrow) { ASSERT_EQ('*', fmt::to_narrow<char>(42)); }
	TEST(int_unsigned) { ASSERT_EQ(42u, fmt::to_unsigned(42)); }
	TEST(int_signed) { ASSERT_EQ(42, fmt::to_signed(42u)); }

	// negatives

	FAIL(int_loss) { (void) fmt::to_narrow<char>(256); }
	FAIL(int_loss_sign) { (void) fmt::to_unsigned(-1); }

	//
	// Text formatting routines
	//

	TEST(fmt_terminated)
	{
		using namespace fmt;
		ASSERT(terminated(string_view(HELLO_WORLD, 13)));
		ASSERT(not terminated(string_view(HELLO_WORLD, 5)));
	}

	TEST(fmt_trim)
	{
		using namespace fmt;
		string whitespace = " \t\n";
		ASSERT(trim(whitespace).empty());
		constexpr auto raw = " \t" HELLO_WORLD "\n";
		auto const u = trim(raw);
		ASSERT(not empty(u));
		ASSERT_EQ(u, HELLO_WORLD);
		ASSERT_NOT_EQ(u, raw);
	}

	TEST(fmt_wide)
	{
		using namespace fmt;
		ASSERT_EQ(to_wstring(string { HELLO_WORLD }), HELLO_WIDE);
		ASSERT_EQ(to_string(wstring { HELLO_WIDE }), HELLO_WORLD);
	}

	TEST(fmt_case)
	{
		using namespace fmt;
		ASSERT_EQ(to_upper(HELLO_WORLD), HELLO_UPPER);
		ASSERT_EQ(to_lower(HELLO_WORLD), HELLO_LOWER);
	}

	TEST(fmt_delimiter)
	{
		using namespace fmt;
		string_view u = "A,B,C,D";
		delimiter del {u, ","};

		string_view_vector t;
		for (auto tok : del)
		{
			auto const n = tok.second - tok.first;
			t.emplace_back(u.substr(tok.first, n));
		}

		ASSERT_EQ(t.size(), 4);
		ASSERT_EQ(t[0], "A");
		ASSERT_EQ(t[1], "B");
		ASSERT_EQ(t[2], "C");
		ASSERT_EQ(t[3], "D");
	}

	TEST(fmt_sequence)
	{
		using namespace fmt;
		string_view u = "A<B>C";
		sequence q {u, {"<", ">"}};
		string_view_vector t;
		for (auto tok : q)
		{
			auto const n = tok.second - tok.first;
			t.emplace_back(u.substr(tok.first, n));
		}

		ASSERT_EQ(t.size(), 3);
		ASSERT_EQ(t[0], "A");
		ASSERT_EQ(t[1], "B");
		ASSERT_EQ(t[2], "C");
	}

	TEST(fmt_tag)
	{
		using namespace fmt;
		string_view u = "bananas, apple, grape, banana";
		string_view_span a { "apple", "banana" };
		string_view_vector t;
		tag(a, u, [&](auto it, auto pos)
		{
			ASSERT_EQ(0, u.compare(pos, it->size(), *it));
			t.emplace_back(*it);
		});

		ASSERT_EQ(t.size(), 3);
		ASSERT_EQ(t[0], "banana");
		ASSERT_EQ(t[1], "apple");
		ASSERT_EQ(t[2], "banana");
	}

	//
	// Operating system environment
	//

	TEST(env_path)
	{
		std::string_view const var = sys::env::get("PATH");
		std::string const val = fmt::path::join(env::paths);
		ASSERT_EQ(var, val);
	}

	std::ofstream& outfile()
	{
		static std::ofstream f { "sys.ini" };
		return f;
	}

	TEST(env_fake)
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
	}

	TEST(xdg_desktop)
	{
		auto& f = outfile();
		f << "[Desktop]" << std::endl;
		f << fmt::key_value("XDG_CURRENT_DESKTOP", xdg::current_desktop) << std::endl;
		f << fmt::key_value("XDG_MENU_PREFIX", xdg::menu_prefix) << std::endl;
		f << fmt::key_value("XDG_APPLICATIONS_MENU", xdg::applications_menu) << std::endl;
	}

	TEST(xdg_data)
	{
		auto& f = outfile();
		f << "[Data Directories]" << std::endl;
		f << fmt::key_value("XDG_RUNDTIME_DIR", xdg::runtime_dir) << std::endl;
		f << fmt::key_value("XDG_DATA_HOME", xdg::data_home) << std::endl;
		f << fmt::key_value("XDG_CONFIG_HOME", xdg::config_home) << std::endl;
		f << fmt::key_value("XDG_CACHE_HOME", xdg::cache_home) << std::endl;
		f << fmt::key_value("XDG_DATA_DIRS", fmt::join(xdg::data_dirs, sys::sep::path)) << std::endl;
		f << fmt::key_value("XDG_CONFIG_DIRS", fmt::join(xdg::config_dirs, sys::sep::path)) << std::endl;
	}

	TEST(xdg_user)
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
	}

	//
	// Operating system processes
	//

	static int hidden() { return 42; }
	dynamic int visible() { return hidden(); }

	TEST(sys_symbol)
	{
		sys::dll lib;
		auto f = lib.sym<int()>("visible");
		ASSERT_NOT_EQ(nullptr, f);
		ASSERT_EQ(f(), hidden());
	}

	TEST(sys_signal)
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

		ASSERT_EQ(caught.size(), 3);
		ASSERT_EQ(caught[0], SIGINT);
		ASSERT_EQ(caught[1], SIGFPE);
		ASSERT_EQ(caught[2], SIGILL);
	}

	//
	// ANSI escape sequence
	//

	TEST(ansi_params)
	{
		std::ostringstream ss;
		ss << sgr::params<1, 2, 3, 4>;
		std::string const s = ss.str();
		ASSERT_EQ(s, "1;2;3;4");
	}

	TEST(ansi_fg)
	{
		std::ostringstream ss;
		ss << sgr::fg_green << "GREEN" << sgr::fg_off;
		std::string const s = ss.str();
		ASSERT_EQ(s, "\x1b[32mGREEN\x1b[39m");
	}

	//
	// Inter-process communications
	//

	TEST(ipc_rev)
	{
		sys::io::pstream ps { "rev" };
		ps << HELLO_WORLD;
		ps.close(0);
		std::string s;
		ASSERT(std::getline(ps, s));
		ASSERT_EQ(s, DLROW_OLLEH);
	}

	TEST(ipc_mem)
	{
		sys::file::descriptor file;
		file.open(__FILE__, sys::file::in);
		sys::file::view map(file.get());

		fmt::string_view const view = map;
		auto pos = view.find("Self referencing find.");
		ASSERT_NOT_EQ(pos, fmt::string_view::npos);
	}
}

