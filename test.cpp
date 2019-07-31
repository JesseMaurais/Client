// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dbg.hpp"
#include "int.hpp"
#include "fmt.hpp"
#include "tag.hpp"
#include "del.hpp"
#include "esc.hpp"
#include "xdg.hpp"
#include "sys.hpp"
#include "net.hpp"
#include "sig.hpp"
#include "dir.hpp"
#include "dll.hpp"
#include "shm.hpp"
#include "fifo.hpp"
#include "pstream.hpp"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <csignal>

#define HELLO_WORLD "Hello, World!"
#define HELLO_WIDE L"Hello, World!"
#define HELLO_UPPER "HELLO, WORLD!"
#define HELLO_LOWER "hello, world!"
#define DLROW_OLLEH "!dlroW ,olleH"

#ifdef NDEBUG
static_assert("Compiling unit tests without debug mode.");
#endif

int main(int argc, char **argv)
{
	(void) argc;
	return dbg::run(argv[1]);
}

namespace
{
	//
	// Sanity test the unit test code itself
	//

	TEST(dbg_sane) { assert(true and not false); }
	TEST(dbg_equality) { assert(true == true); }
	TEST(dbg_inequality) { assert(true != false); }

	// negatives

	FAIL(dbg_assert) { assert(false); }
	FAIL(dbg_except) { throw std::runtime_error("Holy Cow!"); }
	FAIL(dbg_equal) { assert(true == false); }
	FAIL(dbg_unequal) { assert(true != true); }
	
	//
	// Checked integer conversions
	//

	TEST(int_narrow) { assert('*' == fmt::to_narrow<char>(42)); }
	TEST(int_unsigned) { assert(42u == fmt::to_unsigned(42)); }
	TEST(int_signed) { assert(42 == fmt::to_signed(42u)); }

	// negatives

	FAIL(int_loss) { (void) fmt::to_narrow<char>(256); }
	FAIL(int_loss_sign) { (void) fmt::to_unsigned(-1); }

	//
	// Text formatting routines
	//

	TEST(fmt_terminated)
	{
		using namespace fmt;
		assert(terminated(string_view(HELLO_WORLD, 13)));
		assert(not terminated(string_view(HELLO_WORLD, 5)));
	}

	TEST(fmt_trim)
	{
		using namespace fmt;
		string whitespace = " \t\n";
		assert(trim(whitespace).empty());
		constexpr auto raw = " \t" HELLO_WORLD "\n";
		auto const u = trim(raw);
		assert(not empty(u));
		assert(u == HELLO_WORLD);
		assert(u != raw);
	}

	TEST(fmt_wide)
	{
		using namespace fmt;
		assert(to_wstring(string { HELLO_WORLD }) == HELLO_WIDE);
		assert(to_string(wstring { HELLO_WIDE }) == HELLO_WORLD);
	}

	TEST(fmt_case)
	{
		using namespace fmt;
		assert(to_upper(HELLO_WORLD) == HELLO_UPPER);
		assert(to_lower(HELLO_WORLD) == HELLO_LOWER);
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

		assert(t.size() == 4);
		assert(t[0] == "A");
		assert(t[1] == "B");
		assert(t[2] == "C");
		assert(t[3] == "D");
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

		assert(t.size() == 3);
		assert(t[0] == "A");
		assert(t[1] == "B");
		assert(t[2] == "C");
	}

	TEST(fmt_tag)
	{
		using namespace fmt;
		string_view u = "bananas, apple, grape, banana";
		string_view_span a { "apple", "banana" };
		string_view_vector t;
		tag(a, u, [&](auto it, auto pos)
		{
			assert(0 == u.compare(pos, it->size(), *it));
			t.emplace_back(*it);
		});

		assert(t.size() == 3);
		assert(t[0] == "banana");
		assert(t[1] == "apple");
		assert(t[2] == "banana");
	}

	//
	// ANSI escape sequence
	//

	TEST(fmt_params)
	{
		std::ostringstream ss;
		ss << fmt::params<1, 2, 3, 4>;
		std::string const s = ss.str();
		assert(s == "1;2;3;4");
	}

	TEST(fmt_fg)
	{
		std::ostringstream ss;
		ss << fmt::fg_green << "GREEN" << fmt::fg_off;
		std::string const s = ss.str();
		assert(s == "\x1b[32mGREEN\x1b[39m");
	}

	//
	// Operating system environment
	//

	std::ofstream& sysini()
	{
		static std::ofstream f { "sys.ini" };
		return f;
	}

	auto kv(fmt::string_view key, fmt::string_view value)
	{
		return fmt::key_value(key, value) + '\n';
	}

	TEST(env_path)
	{
		auto var = sys::env::get("PATH");
		auto val = fmt::path::join(env::paths);
		assert(var == val);

		#ifdef _WIN32
		{
			val = sys::env::value("%PATH%");
		}
		#else
		{
			val = sys::env::value("$PATH");
		}
		#endif

		var = sys::env::get("PATH");
		assert(var == val);
	}

	TEST(env_vars)
	{
		sysini()
		<< "[Fake Environment]\n"
		<< kv("HOME", env::home)
		<< kv("USER", env::user)
		<< kv("ROOT", env::root)
		<< kv("DOMAIN", env::domain)
		<< kv("HOST", env::host)
		<< kv("PWD", env::pwd)
		<< kv("LANG", env::lang)
		<< kv("SHELL", env::shell)
		<< kv("TMPDIR", env::tmpdir)
		<< kv("ROOTDIR", env::rootdir)
		<< kv("DESKTOP", env::desktop)
		<< kv("PROMPT", env::prompt)
		<< std::endl;
	}

	TEST(env_dir)
	{
		sysini()
		<< "[Common Directores]\n"
		<< kv("LIB", fmt::dir::join(env::dir::lib))
		<< kv("SHARE", fmt::dir::join(env::dir::share))
		<< kv("INCLUDE", fmt::dir::join(env::dir::include))
		<< std::endl;
	}

	TEST(xdg_desktop)
	{
		sysini()
		<< "[Desktop]\n"
		<< kv("XDG_CURRENT_DESKTOP", xdg::current_desktop)
		<< kv("XDG_MENU_PREFIX", xdg::menu_prefix)
		<< kv("XDG_APPLICATIONS_MENU", xdg::applications_menu)
		<< std::endl;
	}

	TEST(xdg_data)
	{
		sysini()
		<< "[Data Directories]\n"
		<< kv("XDG_RUNDTIME_DIR", xdg::runtime_dir)
		<< kv("XDG_DATA_HOME", xdg::data_home)
		<< kv("XDG_CONFIG_HOME", xdg::config_home)
		<< kv("XDG_CACHE_HOME", xdg::cache_home)
		<< kv("XDG_DATA_DIRS", fmt::path::join(xdg::data_dirs))
		<< kv("XDG_CONFIG_DIRS", fmt::path::join(xdg::config_dirs))
		<< std::endl;
	}

	TEST(xdg_user)
	{
		sysini()
		<< "[User Directories]\n"
		<< kv("XDG_DESKTOP_DIR", xdg::desktop_dir)
		<< kv("XDG_DOCUMENTS_DIR", xdg::documents_dir)
		<< kv("XDG_DOWNLOAD_DIR", xdg::download_dir)
		<< kv("XDG_MUSIC_DIR", xdg::music_dir)
		<< kv("XDG_PICTURES_DIR", xdg::pictures_dir)
		<< kv("XDG_PUBLICSHARE_DIR", xdg::publicshare_dir)
		<< kv("XDG_TEMPLATES_DIR", xdg::templates_dir)
		<< kv("XDG_VIDEOS_DIR", xdg::videos_dir)
		<< std::endl;
	}

	//
	// Operating system processes
	//

	static int hidden() { return 42; }
	dynamic int visible() { return hidden(); }

	TEST(sys_dll)
	{
		sys::dll self;
		auto f = self.sym<int()>("visible");
		assert(nullptr != f);
		assert(f() == hidden());
	}

	TEST(sys_sig)
	{
		std::vector<int> caught;
		for (int const n : { SIGINT, SIGFPE, SIGILL })
		{
			sys::sig::scope handle(n, [&](int on)
			{
				caught.push_back(on);
			});
			std::raise(n);
		}

		assert(caught.size() == 3);
		assert(caught[0] == SIGINT);
		assert(caught[1] == SIGFPE);
		assert(caught[2] == SIGILL);
	}

	//
	// Inter-process communications
	//

	TEST(ipc_rev)
	{
		io::pstream ps { "rev" };
		ps << HELLO_WORLD;
		ps.close(0);
		std::string s;
		assert(std::getline(ps, s));
		assert(s == DLROW_OLLEH);
	}

	TEST(ipc_map)
	{
		sys::file::descriptor file;
		file.open(__FILE__, sys::file::rd);
		sys::file::view map;
		assert(not map.open(file.get()));
		fmt::string_view const view = map;
		auto pos = view.find("Self referencing find.");
		assert(pos != fmt::string_view::npos);
		assert(not map.close());
	}
}

