// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dbg.hpp"  // Debug and unit test tools
#include "fmt.hpp"  // Character string formatting
#include "int.hpp"  // Numeric conversions
#include "ini.hpp"  // Desktop configuration parsing
#include "opt.hpp"  // Query user options
#include "tag.hpp"  // Tag parsing
#include "del.hpp"  // Parsing with delimiters
#include "ctl.hpp"  // ASCII control sequences
#include "ios.hpp"  // I/O Streams
#include "sys.hpp"  // System interfaces
#include "net.hpp"  // Network sockets
#include "sig.hpp"  // Signal handling
#include "env.hpp"  // Environment variables
#include "dir.hpp"  // Directory operators
#include "usr.hpp"  // User directories
#include "cpu.hpp"  // CPU information
#include "dll.hpp"  // Dynamic libraries
#include "shm.hpp"  // Shared memory
#include "fds.hpp"  // File descriptor streams
#include "ips.hpp"  // Inter process streams
#include "fifo.hpp" // Named pipes

#define HELLO_WORLD "Hello, World!"
#define HELLO_WIDE L"Hello, World!"
#define HELLO_UPPER "HELLO, WORLD!"
#define HELLO_LOWER "hello, world!"
#define DLROW_OLLEH "!dlroW ,olleH"

#ifdef NDEBUG
static_assert(false, "Compiling unit tests without debug mode.");
#endif

env::opt::view const env::opt::application = "Modify";

int main(int argc, char **argv)
{
	env::opt::set(argc, argv);
	return dbg::run(argv[1]);
}

namespace
{
	//
	// Sanity test the unit test code itself
	//

	//TEST(dbg_sane) { assert(true and not false); }
	//TEST(dbg_equality) { assert(true == true); }
	//TEST(dbg_inequality) { assert(true != false); }

	// negatives

	//FAIL(dbg_assert) { assert(false); }
	//FAIL(dbg_except) { throw std::runtime_error("Holy Cow!"); }
	//FAIL(dbg_equal) { assert(true == false); }
	//FAIL(dbg_unequal) { assert(true != true); }
	
	//
	// Checked integer and floating point conversions
	//
	
	TEST(int_narrow) { assert('*' == fmt::to_narrow<char>(42)); }
	TEST(int_unsigned) { assert(42u == fmt::to_unsigned(42)); }
	TEST(int_signed) { assert(42 == fmt::to_signed(42u)); }
	TEST(int_format) { assert(42 == fmt::to_long("42")); }
	TEST(int_oct) { assert(42 == fmt::to_long("52", 8)); }
	TEST(int_hex) { assert(42 == fmt::to_long("2A", 16)); }
	TEST(int_fp) { assert(0.42f == fmt::to_float("0.42f")); }
	TEST(int_nan) { assert(std::isnan(fmt::to_float("nan"))); }
	TEST(int_inf) { assert(std::isinf(fmt::to_double("inf"))); }

	// negatives

	FAIL(int_loss) { (void) fmt::to_narrow<char>(256); }
	FAIL(int_loss_sign) { (void) fmt::to_unsigned(-1); }
	FAIL(int_loss_hex) { (void) fmt::to_long("$"); }

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

	TEST(fmt_ini)
	{
		auto const path = fmt::dir::join(env::pwd, "Tools.ini");
		std::fstream file(path);
		ini keys;
		file >> keys;

		ini::pair entry { "NMAKE", "MAKECONFIG" };
		auto const value = keys.get(entry);
		assert(not empty(value));
		assert(fmt::npos != value.find("/D_NMAKE"));
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
		return fmt::entry(key, value) + fmt::eol;
	}

	TEST(env_path)
	{
		auto var = sys::env::get("PATH");
		auto val = fmt::path::join(env::paths);
		assert(var == val);

		val = sys::env::value("$PATH");
		var = sys::env::get("PATH");
		assert(var == val);
	}

	TEST(env_dir_enum)
	{
		fmt::string_vector list;
		env::dir::find(env::pwd, [&](auto path)
		{
			list.emplace_back(path);
			return success;
		});
		assert(find(begin(list), end(list), __FILE__) != end(list));
	}

	TEST(env_dir_make)
	{
		auto const d = fmt::dir::join(env::tmpdir, "my", "test", "dir");
		auto const stem = env::dir::make(d);
		assert(not empty(stem));
		assert(not env::dir::fail(d));
		assert(not env::dir::fail(stem));
		assert(not fail(env::dir::remove(stem)));
		{
			env::dir::tmp tmp(d);
			assert(not env::dir::fail(d));
		}
		assert(env::dir::fail(d));
	}

	TEST(env_opt_at)
	{
		assert(env::opt::set("ptr", "@key"));
		assert(env::opt::set("key", HELLO_WORLD));
		auto const value = env::opt::get("ptr");
		assert(0 == value.compare(HELLO_WORLD));
	}

	TEST(env_vars)
	{
		sysini()
		<< "[Fake Environment]\n"
		<< kv("home", env::home)
		<< kv("user", env::user)
		<< kv("root", env::root)
		<< kv("domain", env::domain)
		<< kv("host", env::host)
		<< kv("pwd", env::pwd)
		<< kv("lang", env::lang)
		<< kv("shell", env::shell)
		<< kv("tmpdir", env::tmpdir)
		<< kv("rootdir", env::rootdir)
		<< kv("desktop", env::desktop)
		<< kv("prompt", env::prompt)
		<< std::endl;
	}

	TEST(env_dev)
	{
		sysini()
		<< "[Development Directores]\n"
		<< kv("lib", fmt::dir::join(env::dir::lib))
		<< kv("share", fmt::dir::join(env::dir::share))
		<< kv("include", fmt::dir::join(env::dir::include))
		<< std::endl;
	}

	TEST(env_opt)
	{
		sysini()
		<< "[Application Options]\n"
		<< kv("name", env::opt::application)
		<< kv("program", env::opt::program)
		<< kv("command-line", fmt::join(env::opt::arguments, " "))
		<< kv("config", env::opt::config)
		<< kv("cache", env::opt::cache)
		<< env::opt::put
		<< std::endl;
	}

	TEST(usr_desktop)
	{
		sysini()
		<< "[Desktop]\n"
		<< kv("current-desktop", env::usr::current_desktop)
		<< kv("menu-prefix", env::usr::menu_prefix)
		<< kv("applications-menu", env::usr::applications_menu)
		<< std::endl;
	}

	TEST(usr_data)
	{
		sysini()
		<< "[Data Directories]\n"
		<< kv("runtime-dir", env::usr::runtime_dir)
		<< kv("data-home", env::usr::data_home)
		<< kv("config-home", env::usr::config_home)
		<< kv("cache-home", env::usr::cache_home)
		<< kv("data-dirs", fmt::path::join(env::usr::data_dirs))
		<< kv("config-dirs", fmt::path::join(env::usr::config_dirs))
		<< std::endl;
	}

	TEST(usr_dir)
	{
		sysini()
		<< "[User Directories]\n"
		<< kv("desktop-dir", env::usr::desktop_dir)
		<< kv("documents-dir", env::usr::documents_dir)
		<< kv("download-dir", env::usr::download_dir)
		<< kv("music-dir", env::usr::music_dir)
		<< kv("pictures-dir", env::usr::pictures_dir)
		<< kv("publicshare-dir", env::usr::publicshare_dir)
		<< kv("template-dir", env::usr::templates_dir)
		<< kv("videos-dir", env::usr::videos_dir)
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

	TEST(sys_cpu)
	{
		sysini()
		<< "[CPU Information]\n"
		<< kv("count", fmt::to_string(sys::cpu::count))
		<< kv("page-size", fmt::to_string(sys::cpu::page_size))
		<< std::endl;
	}

	//
	// I/O Streams
	//

	TEST(ios_fds)
	{
		io::ifdstream in { __FILE__ };
		assert(in);
		fmt::string line;
		fmt::size_type n = 0;
		while (std::getline(in, line))
		{
			assert(__LINE__ != ++n or line.find("assert") != fmt::npos);
		}
		assert(__LINE__ < n);
	}

	/* Break WIN32
	TEST(ios_ipc)
	{
		io::pstream ps { "rev" };
		ps << HELLO_WORLD;
		ps.close(0);
		std::string s;
		assert(std::getline(ps, s));
		assert(s == DLROW_OLLEH);
	}

	TEST(ios_map)
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
	*/
}

