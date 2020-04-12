// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dbg.hpp"  // Debug and unit test tools
#include "err.hpp"  // System error logging
#include "type.hpp" // String views and spans
#include "fmt.hpp"  // Character string formatting
#include "dig.hpp"  // Numeric conversions
#include "gc.hpp"   // Stream control sequences
#include "fd.hpp"   // File descriptor streams
#include "pid.hpp"  // Inter process streams
#include "sys.hpp"  // System interfaces
#include "net.hpp"  // Network sockets
#include "sig.hpp"  // Signal handling
#include "dll.hpp"  // Dynamic libraries
#include "shm.hpp"  // Shared memory
#include "fifo.hpp" // Named pipes
#include "env.hpp"  // Environment variables
#include "dir.hpp"  // Directory operators
#include "usr.hpp"  // User directories
#include "opt.hpp"  // Query user options
#include "ini.hpp"  // Desktop configuration parsing
#include "desktop.hpp"

#define HELLO_WORLD "Hello, World!"
#define HELLO_WIDE L"Hello, World!"
#define HELLO_UPPER "HELLO, WORLD!"
#define HELLO_LOWER "hello, world!"
#define DLROW_OLLEH "!dlroW ,olleH"

#ifdef NDEBUG
static_assert(false, "Compiling unit tests without debug mode.");
#endif

env::opt::view const env::opt::application = "PoTest";

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
	//FAIL(dbg_equal) { assert(true == false); }
	//FAIL(dbg_unequal) { assert(true != true); }
	FAIL(dbg_except) { throw std::runtime_error("Holy Cow!"); }
	FAIL(dbg_warn) { sys::warn(here, "Don't do that!"); }

	//
	// Checked integer and floating point conversions
	//

	TEST(fmt_narrow) { assert('*' == fmt::to_narrow<char>(42)); }
	TEST(fmt_unsigned) { assert(42u == fmt::to_unsigned(42)); }
	TEST(fmt_signed) { assert(42 == fmt::to_signed(42u)); }
	TEST(fmt_dec) { assert(42 == fmt::to_long("42")); }
	TEST(fmt_oct) { assert(42 == fmt::to_long("52", 8)); }
	TEST(fmt_hex) { assert(42 == fmt::to_long("2A", 16)); }
	TEST(fmt_float) { assert(0.42f == fmt::to_float("0.42f")); }
	TEST(fmt_nan) { assert(std::isnan(fmt::to_float("nan"))); }
	TEST(fmt_inf) { assert(std::isinf(fmt::to_double("inf"))); }

	// negatives

	FAIL(fmt_loss) { (void) fmt::to_narrow<char>(256); }
	FAIL(fmt_loss_sign) { (void) fmt::to_unsigned(-1); }
	FAIL(fmt_loss_hex) { (void) fmt::to_long("$"); }

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

	TEST(fmt_brace)
	{
		using namespace fmt;
		pair<size_type> pos;
		pos = embrace("<A<B>C>", "<>");
		assert(0 == pos.first);
		assert(6 == pos.second);
		pos = embrace("<A<B>C>", "<B>");
		assert(0 == pos.first);
		assert(3 == pos.second);
		pos = embrace("A[B]C[D]", "[D]");
		assert(1 == pos.first);
		assert(3 == pos.second);
		pos = embrace("{A<B}C}", "<>");
		assert(2 == pos.first);
		assert(npos == pos.second);
		pos = embrace("{A{B>C}", "<>");
		assert(npos == pos.first);
		assert(npos == pos.second);
		pos = embrace("&amp;", "&;");
		assert(0 == pos.first);
		assert(4 == pos.second);
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
	// File formats
	//

	TEST(doc_ini)
	{
		auto const path = fmt::dir::join({env::pwd, "Tools.ini"});
		std::fstream file(path);
		doc::ini keys;
		file >> keys;

		doc::ini::pair entry { "NMAKE", "MAKECONFIG" };
		auto const value = keys.get(entry);
		assert(not empty(value));
		assert(fmt::npos != value.find("/D_NMAKE"));
	}

	//
	// Operating system environment
	//

	std::ofstream& sysini()
	{
		static std::ofstream f { "Tests.ini" };
		return f;
	}

	auto kv(fmt::string_view key, fmt::string_view value)
	{
		return fmt::entry(key, value) + fmt::eol;
	}

	TEST(env_path)
	{
		auto var = env::var::get("PATH");
		auto val = fmt::path::join(env::paths);
		assert(var == val);

		val = env::var::value("$PATH");
		var = env::var::get("PATH");
		assert(var == val);
	}

	TEST(env_dir_enum)
	{
		std::vector<fmt::string> list;
		auto parts = fmt::dir::split(__FILE__);
		auto const name = parts.back();
		parts.pop_back();
		auto const folder = fmt::dir::join(parts);
		auto const srcdir = fmt::dir::join({env::pwd, folder});
		env::dir::find(srcdir, [&](auto path)
		{
			list.emplace_back(path);
			return success;
		});
		assert(find(begin(list), end(list), name) != end(list));
	}

	TEST(env_dir_make)
	{
		auto const d = fmt::dir::join({env::tmpdir, "my", "test", "dir"});
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
		<< kv("session", env::session)
		<< kv("prompt", env::prompt)
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
		<< kv("runtime-dir", env::usr::run_dir)
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
		<< kv("count", fmt::to_string(env::sys::cpu::count))
		<< kv("page-size", fmt::to_string(env::sys::cpu::page_size))
		<< std::endl;
	}

	TEST(_sys_ipc_rev)
	{
		std::string s;
		assert(std::getline(std::cin, s));
		std::reverse(s.begin(), s.end());
		std::cout << s << std::endl;
	}

	TEST(sys_ipc)
	{
		fmt::pstream ps { env::opt::arg(0), "_sys_ipc_rev" };
		ps << HELLO_WORLD << std::endl;
		std::string s;
		assert(std::getline(ps, s));
		assert(DLROW_OLLEH == s);
	}

	TEST(sys_map)
	{
		env::file::descriptor file(__FILE__, env::file::rd);
		assert(not env::file::fail(file.get()));
		std::size_t sz = 0;
		auto const mem = env::file::make_shm(file.get(), 0, 0, env::file::rd, &sz);
		assert(nullptr != mem);
		auto const c = static_cast<char*>(mem.get());
		fmt::string_view const view(c, sz);
		auto pos = view.find("Self referencing find.");
		assert(pos != fmt::npos);
	}

	//
	// I/O Streams
	//

	TEST(ios_fds)
	{
		fmt::ifdstream in { __FILE__ };
		assert(in);
		fmt::string line;
		fmt::size_type n = 0;
		while (std::getline(in, line))
		{
			assert(__LINE__ != ++n or line.find("assert") != fmt::npos);
		}
		assert(__LINE__ < n);
	}

	TEST(desktop_open)
	{
		auto const edit = env::desktop::open(__FILE__);
	}
}
