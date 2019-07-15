#include "dir.hpp"
#include "fmt.hpp"
#include "sys.hpp"
#include "ptr.hpp"
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
		if (bit & ok)
		{
			mask |= F_OK;
		}
		if (bit & run)
		{
			mask |= F_X;
		}
		if (bit & read)
		{
			mask |= F_R;
		}
		if (bit & write)
		{
			mask |= F_W;
		}
		return mask;
	}
}

namespace sys::dir
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

namespace sys::path
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
	struct : env::list
	{
		operator fmt::string_view_span() const final
		{
			return env::path;
		}

	} BIN;

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
			t = sys::path::split(u);
			return t;
		}

	} SHARE;

	struct : env::list
	{
		operator fmt::string_view_span() const final
		{
			static fmt::string_view_vector t;
			auto const u = sys::env::get("INCLUDE");
			t = sys::path::split(u);
			return t;
		}

	} INCLUDE;
}

namespace env::dir
{
	env::list const& bin = BIN;
	env::list const& share = SHARE;
	env::list const& include = INCLUDE;

	bool find(fmt::string path, view check)
	{
		auto const str = path.c_str();
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
			if (auto const ent = readdir(dir); ent)
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

	view mode(sys::file::mode mask)
	{
		auto const flags = sys::file::convert(mask);
		return [flags](dirent const* ent)
		{
			return not sys::fail(sys::access(ent->d_name, flags));
		};
	}

	view regex(fmt::string_view pattern)
	{
		auto const expr = std::regex(pattern);
		return [expr](dirent const* ent)
		{
			std::match_results res;
			return std::regex_search(ent->d_name, res, expr);
		};
	}

	view name(fmt::string& buf)
	{
		return [&](dirent const* ent)
		{
			buf.assign(ent->d_name);
			return true;
		};
	}
}

