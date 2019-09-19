// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dev.hpp"
#include "dir.hpp"
#include "err.hpp"
#include "fmt.hpp"
#include "usr.hpp"
#include "opt.hpp"
#include "os.hpp"
#include "cc.hpp"

namespace
{
	struct : env::view
	{
		operator fmt::string_view() const final
		{
			static auto const rundir = env::opt::directory(env::usr::runtime_dir);
			static env::dir::tmp tmp(rundir);
			return rundir;
		}

	} RUN;

	struct : env::span
	{
		operator fmt::string_view_span() const final
		{
			static fmt::string_view_vector t;
			fmt::string_view u = env::sys::get("LIB");
			t = fmt::dir::split(u);
			return t;
		}

	} LIB;

	struct : env::span
	{
		static auto get()
		{
			#ifdef __WIN32__
			{
				return env::sys::get("PATH");
			}
			#else
			#ifdef __AIX__
			{
				return env::sys::get("LIBPATH");
			}
			#else
			#ifdef __HPUX__
			{
				return env::sys::get("SHLIB_PATH");
			}
			#else
			{
				return env::sys::get("LD_LIBRARY_PATH");
			}
			#endif
			#endif
			#endif
		}

		operator fmt::string_view_span() const final
		{
			static fmt::string_view_vector t;
			auto const u = get();
			t = fmt::path::split(u);
			return t;
		}

	} SHARE;

	struct : env::span
	{
		operator fmt::string_view_span() const final
		{
			static fmt::string_view_vector t;
			auto const u = env::opt::get("INCLUDE");
			t = fmt::path::split(u);
			return t;
		}

	} INCLUDE;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			return env::opt::get("CFLAGS");
		}

	} CFLAGS;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			auto u = env::sys::get("CXX");
			if (empty(u))
			{
				u = "c++";
			}
			return u;
		}

	} CC;
}

namespace env::dev
{
	env::view const& run = RUN;
	env::span const& lib = LIB;
	env::span const& share = SHARE;
	env::span const& include = INCLUDE;
	env::view const& cflags = CFLAGS;
	env::view const& cc = CC;
}


