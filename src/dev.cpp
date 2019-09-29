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
	struct : env::span
	{
		operator fmt::span<fmt::view>() const final
		{
			static std::vector<fmt::view> t;
			auto u = env::opt::get("LIB");
			t = fmt::dir::split(u);
			return t;
		}

	} LIB;

	struct : env::span
	{
		operator fmt::span<fmt::view>() const final
		{
			static fmt::array table;
			if (empty(table))
			{
				fmt::view value;
				for (auto c : {"LD_LIBRARY_PATH", "LIBPATH", "LIB", "PATH"})
				{
					value = env::var::get(c);
					if (not empty(value))
					{
						break;
					}
				}
				table = fmt::path::split(value);
			}
			return table;
		}

	} SHARE;

	struct : env::span
	{
		operator fmt::span<fmt::view>() const final
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
			auto u = env::opt::get("CXX");
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
	env::span const& lib = LIB;
	env::span const& share = SHARE;
	env::span const& include = INCLUDE;
	env::view const& lflags = LFLAGS;
	env::view const& cflags = CFLAGS;
	env::view const& cc = CC;
}


