#include "xdg.hpp"
#include "fmt.hpp"
#include "ini.hpp"
#include "sys.hpp"

namespace
{
	struct : env::view
	{
		operator std::string_view() const final
		{
			return sys::env::get("XDG_CURRENT_DESKTOP");
		}

	} XDG_CURRENT_DESKTOP;

	struct : env::view
	{
		operator std::string_view() const final
		{
			std::string_view u = sys::env::get("XDG_MENU_PREFIX");
			if (empty(u))
			{
				static auto prefix = fmt::to_lower(xdg::current_desktop) + '-';
				u = prefix;
			}
			return u;
		}

	} XDG_MENU_PREFIX;

	struct : env::view
	{
		operator std::string_view() const final
		{
			return sys::env::get("XDG_RUNTIME_DIR");
		}

	} XDG_RUNTIME_DIR;

	struct : env::view
	{
		operator std::string_view() const final
		{
			std::string_view u = sys::env::get("XDG_DATA_HOME");
			if (empty(u))
			{
				static std::string dir;
				if (empty(dir))
				{
					fmt::span_view const p { env::home, ".local", "share" };
					u = fmt::join(p, sys::sep::dir);
				}
				u = dir;
			}
			return u;
		}

	} XDG_DATA_HOME;

	struct : env::view
	{
		operator std::string_view() const final
		{
			std::string_view u = sys::env::get("XDG_CONFIG_HOME");
			if (empty(u))
			{
				static std::string dir;
				if (empty(dir))
				{
					fmt::span_view const p { env::home, ".config" };
					dir = fmt::join(p, sys::sep::dir);
				}
				u= dir;
			}
			return u;
		}

	} XDG_CONFIG_HOME;

	struct : env::view
	{
		operator std::string_view() const final
		{
			std::string_view u = sys::env::get("XDG_CACHE_HOME");
			if (empty(u))
			{
				static std::string dir;
				if (empty(dir))
				{
					fmt::span_view const p { env::home, ".cache" };
					dir = fmt::join(p, sys::sep::dir);
				}
				u = dir;
			}
			return view;
		}

	} XDG_CACHE_HOME;

	struct : env::list
	{
		operator std::vector<std::string_view>() const final
		{
			std::string_view u = sys::env::get("XDG_DATA_DIRS");
			if (empty(u))
			{
				if constexpr (sys::POSIX)
				{
					u = "/usr/local/share/:/usr/share/";
				}
				else
				if constexpr (sys::WIN32)
				{
					static std::string dir;
					if (empty(dir))
					{
						std::string const data = sys::env::get("APPDATA");
						std::string const local = sys::env::get("LOCALAPPDATA");
						dir = fmt::join({ data, local }, sys::sep::dir);
					}
					u = dir;
				}
			}
			return fmt::split(view, sys::sep::dir);
		}

	} XDG_DATA_DIRS;

	struct : env::list
	{
		operator std::vector<std::string_view>() const final
		{
			std::string_view u = sys::env::get("XDG_CONFIG_DIRS");
			if (empty(u))
			{
				if constexpr (sys::POSIX)
				{
					u = "/etc/xdg";
				}
				else
				if constexpr (sys::WIN32)
				{
					u = sys::env::get("ALLUSERSPROFILE");
				}
			}
			return fmt::split(u, sys::sep::dir);
		}

	} XDG_CONFIG_DIRS;

	std::string_view cached_dirs(std::string_view u)
	{
		static ini::map data;
		if (empty(data))
		{
			fmt::span_view const p { xdg::config_home, "user-dirs.dirs" };
			auto const path = fmt::join(p, sys::sep::dir);
			std::istream in(path);
			std::string s;
			while (ini::getline(in, s))
			{
				fmt::pair p = fmt::key_value(s);
				p.second = sys::env::eval(p.second);
				data.emplace(p);
			}
		}
		return data(u);
	}

	struct : env::view
	{
		constexpr auto var = "XDG_DESKTOP_DIR";

		operator std::string_view() const final
		{
			std::string_view u = sys::env::get(var);
			if (empty(u))
			{
				 u = cached_dirs(var)
			}
			return u;
		}

	} XDG_DESKTOP_DIR;
}

namespace xdg
{
	env::view const& current_desktop = XDG_CURRENT_DESKTOP;
	env::view const& menu_prefix = XDG_MENU_PREFIX;
	env::view const& runtime_dir = XDG_RUNTIME_DIR;
	env::view const& data_home = XDG_DATA_HOME;
	env::view const& config_home = XDG_CONFIG_HOME;
	env::view const& cache_home = XDG_CACHE_HOME;
	env::list const& data_dirs = XDG_DATA_DIRS;
	env::list const& config_dirs = XDG_CONFIG_DIRS;
	env::view const& desktop_dir = XDG_DESKTOP_DIR;
}

