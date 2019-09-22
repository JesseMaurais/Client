// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dev.hpp"
#include "dir.hpp"
#include "err.hpp"
#include "fmt.hpp"
#include "usr.hpp"
#include "opt.hpp"

namespace
{
	struct : env::view
	{
		operator fmt::view() const final
		{
			static auto const rundir = env::opt::dir(env::usr::run_dir);
			static auto tmp = env::dir::tmp(rundir);
			return rundir;
		}

	} RUN;

	struct : env::span
	{
		operator fmt::span() const final
		{
			static std::vector<fmt::view> t;
			auto u = env::sys::get("LIB");
			t = fmt::dir::split(u);
			return t;
		}

	} LIB;

	struct : env::span
	{
		static auto get()
		{
			fmt::view value;
			for (auto c : {"LD_LIBRARY_PATH", "LIBPATH", "LIB", "PATH"})
			{
				value = env::sys::get(c);
				if (not empty(value)) break;
			}
			return value;
		}

		operator fmt::span() const final
		{
			static std::vector<fmt::view> t;
			auto const u = get();
			t = fmt::path::split(u);
			return t;
		}

	} SHARE;

	struct : env::span
	{
		operator fmt::span() const final
		{
			static std::vector<fmt::view> t;
			auto const u = env::opt::get("INCLUDE");
			t = fmt::path::split(u);
			return t;
		}

	} INCLUDE;

	struct : env::view
	{
		operator fmt::view() const final
		{
			return env::opt::get("CFLAGS");
		}

	} CFLAGS;

	struct : env::view
	{
		operator fmt::view() const final
		{
			return env::opt::get("LDFLAGS");
		}

	} LFLAGS;

	struct : env::view
	{
		operator fmt::view() const final
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
	env::view const& lflags = LFLAGS;
	env::view const& cflags = CFLAGS;
	env::view const& cc = CC;
}


