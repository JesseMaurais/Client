// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dir.hpp"
#include "err.hpp"
#include "fmt.hpp"
#include "ptr.hpp"
#include "xdg.hpp"
#include "sys.hpp"
#include "os.hpp"
#include <regex>

#ifdef _WIN32
# include "win.hpp"
#else
# include "uni.hpp"
#endif

namespace fmt::dir
{
	fmt::string impl::join(fmt::string_view_span p)
	{
		return fmt::lc.join(p, sys::sep::dir);
	}

	fmt::string_view_vector split(fmt::string_view u)
	{
		return fmt::split(u, sys::sep::dir);
	}
}

namespace fmt::path
{
	fmt::string impl::join(fmt::string_view_span p)
	{
		return fmt::lc.join(p, sys::sep::path);
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
			constexpr sys::mode_t um = S_IRUSR | S_IWUSR | S_IXUSR;
			fmt::string_view u = xdg::runtime_dir;
			const auto s = u.data();
			
			if (sys::fail(sys::access(s, F_OK)))
			{
				if (sys::fail(sys::mkdir(s, um)))
				{
					sys::err(here, "mkdir", u);
				}
			}
			else
			if (sys::fail(sys::chmod(s, um)))
			{
				sys::err(here, "chmod", u);
			}
			return u;
		}

	} RUN;

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
			#ifdef __WIN32__
			{
				return sys::env::get("PATH");
			}
			#else
			#ifdef __AIX__
			{
				return sys::env::get("LIBPATH");
			}
			#else
			#ifdef __HPUX__
			{
				return sys::env::get("SHLIB_PATH");
			}
			#else
			{
				return sys::env::get("LD_LIBRARY_PATH");
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
	env::list const& lib = LIB;
	env::list const& share = SHARE;
	env::list const& include = INCLUDE;

	bool find(fmt::string_view path, view visitor)
	{
		auto const buf = fmt::to_string(path);
		auto const s = buf.c_str();

		#ifdef _WIN32
		{
			sys::win::files dir(s);
			if (dir.h) do
			{
				if (visitor(dir.cFileName))
				{
					return true;
				}
			}
			while (++dir);
			return false;
		}
		#else
		{
			sys::uni::files dir(s);
			while (dir.ptr) // as if
			{
				auto const ent = ++dir;
				if (ent)
				{
					if (visitor(ent->d_name))
					{
						return true;
					}
				}
				else break;
			}
			return false;
		}
		#endif
	}

	view mode(sys::file::mode bit)
	{
		auto const flags = sys::file::convert(bit);
		return [=](fmt::string_view u)
		{
			auto const s = u.data();
			if (sys::fail(sys::access(s, flags)))
			{
				return false;
			}
			#ifdef _WIN32
			{
				if (bit & sys::file::ex)
				{
					DWORD type;
					BOOL ok = GetBinaryType(s, &type);
					return TRUE == ok;
				}
			}
			#endif
			return true;
		};
	}

	view match(fmt::string_view u)
	{
		auto const buf = fmt::to_string(u);
		auto const x = std::regex(buf);
		return [x](fmt::string_view u)
		{
			std::cmatch m;
			auto const s = u.data();
			return std::regex_search(s, m, x);
		};
	}

	view insert(fmt::string_vector& buf)
	{
		return [&](fmt::string_view u)
		{
			auto s = fmt::to_string(u);
			buf.emplace_back(move(s));
			return false;
		};
	}

	view copy(fmt::string& buf)
	{
		return [&](fmt::string_view u)
		{
			buf = fmt::to_string(u);
			return false;
		};
	}
}

