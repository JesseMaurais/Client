// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dir.hpp"
#include "err.hpp"
#include "fmt.hpp"
#include "sys.hpp"
#include "ptr.hpp"
#include "xdg.hpp"
#include <regex>

#ifdef _WIN32
#include "dirent.h"
#else
#include <dirent.h>
#endif

namespace sys::file
{
	int convert(mode bit)
	{
		int mask = 0;
		if (bit & mode::ok)
		{
			mask |= F_OK;
		}
		if (bit & mode::run)
		{
			mask |= X_OK;
		}
		if (bit & mode::read)
		{
			mask |= R_OK;
		}
		if (bit & mode::write)
		{
			mask |= W_OK;
		}
		return mask;
	}
}

namespace fmt::dir
{
	fmt::string join(fmt::string_view_span p)
	{
		// todo: check for illegal characters
		return fmt::join(p, sys::sep::dir);
	}

	fmt::string_view_vector split(fmt::string_view u)
	{
		return fmt::split(u, sys::sep::dir);
	}
}

namespace fmt::path
{
	fmt::string join(fmt::string_view_span p)
	{
		// todo: check for illegal characters
		return fmt::join(p, sys::sep::path);
	}

	fmt::string_view_vector split(fmt::string_view u)
	{
		return fmt::split(u, sys::sep::path);
	}
}

namespace
{
	struct : env::view
	{
		operator fmt::string_view() const final
		{
			return xdg::runtime_dir;
		}

	} RUN;

	struct : env::list
	{
		operator fmt::string_view_span() const final
		{
			return env::path;
		}

	} BIN;

	struct : env::list
	{
		operator fmt::string_view_span() const final
		{
			static fmt::string_view_vector t;
			fmt::string_view u = sys::env::get("LIB");
			t = fmt::dir::split(u);
			return t;
		}

	} LIB;

	struct : env::list
	{
		static auto get()
		{
			if constexpr (sys::win32)
			{
				return sys::env::get("PATH");
			}
			else
			if constexpr (sys::aix)
			{
				return sys::env::get("LIBPATH");
			}
			else
			if constexpr (sys::hpux)
			{
				return sys::env::get("SHLIB_PATH");
			}
			else
			if constexpr (sys::posix)
			{
				return sys::env::get("LD_LIBRARY_PATH");
			}
		}

		operator fmt::string_view_span() const final
		{
			static fmt::string_view_vector t;
			auto const u = get();
			t = fmt::path::split(u);
			return t;
		}

	} SHARE;

	struct : env::list
	{
		operator fmt::string_view_span() const final
		{
			static fmt::string_view_vector t;
			auto const u = sys::env::get("INCLUDE");
			t = fmt::path::split(u);
			return t;
		}

	} INCLUDE;
}

namespace env::dir
{
	env::view const& run = RUN;
	env::list const& bin = BIN;
	env::list const& lib = LIB;
	env::list const& share = SHARE;
	env::list const& include = INCLUDE;

	bool find(fmt::string_view path, mask check)
	{
		auto const buf = fmt::to_string(path);
		auto const str = buf.c_str(); // terminated
		auto const dir = ptr::make(opendir(str), +[](DIR* dir)
		{
			if (nullptr == dir)
			{
				sys::perror("opendir");
			}
			else
			if (sys::fail(closedir(dir)))
			{
				sys::perror("closedir");
			}
		});

		while (dir) // as if
		{
			if (auto const ent = readdir(dir.get()); ent)
			{
				if (check(ent))
				{
					return true;
				}
			}
			else break;
		}
		return false;
	}

	mask mode(sys::file::mode bits)
	{
		auto const flags = sys::file::convert(bits);
		return [flags](dirent const* ent)
		{
			return not sys::fail(sys::access(ent->d_name, flags));
		};
	}

	mask regex(fmt::string_view pattern)
	{
		auto const buf = fmt::to_string(pattern);
		auto const expr = std::regex(buf);
		return [expr](dirent const* ent)
		{
			std::cmatch match;
			return std::regex_search(ent->d_name, match, expr);
		};
	}

	mask name(fmt::string& buf)
	{
		return [&](dirent const* ent)
		{
			buf.assign(ent->d_name);
			return true;
		};
	}
}

