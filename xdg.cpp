#include "xdg.hpp"
#include "fmt.hpp"
#include "ini.hpp"
#include "sys.hpp"

namespace
{
	struct CURRENT_DESKTOP : env::view
	{
		operator std::string_view() const final
		{
			return sys::env::get("XDG_CURRENT_DESKTOP");
		}
	};

	struct MENU_PREFIX : env::view
	{
		operator std::string_view() const final
		{
			std::string_view u = sys::env::get("XDG_MENU_PREFIX");
			if (empty(u))
			{
				static std::string prefix;
				if (empty(prefix))
				{
					prefix = to_lower(xdg::current_desktop) + '-';
				}
				u = prefix;
			}
			return u;
		}
	};

	struct RUNTIME_DIR : env::view
	{
		operator std::string_view() const final
		{
			return sys::env::get("XDG_RUNTIME_DIR");
		}
	};

	struct DATA_HOME : env::view
	{
		operator std::string_view() const final
		{
			std::string_view u = sys::env::get("XDG_DATA_HOME");
			if (empty(u))
			{
				static std::string dir;
				if (empty(dir))
				{
					fmt::string_span const p { env::home, ".local", "share" };
					u = fmt::join(p, sys::sep::dir);
				}
				u = dir;
			}
			return u;
		}
	};

	struct CONFIG_HOME : env::view
	{
		operator std::string_view() const final
		{
			std::string_view u = sys::env::get("XDG_CONFIG_HOME");
			if (empty(u))
			{
				static std::string dir;
				if (empty(dir))
				{
					fmt::string_span const p { env::home, ".config" };
					dir = fmt::join(p, sys::sep::dir);
				}
				u= dir;
			}
			return u;
		}
	};

	struct CACHE_HOME : env::view
	{
		operator std::string_view() const final
		{
			std::string_view u = sys::env::get("XDG_CACHE_HOME");
			if (empty(u))
			{
				static std::string dir;
				if (empty(dir))
				{
					fmt::string_span const span { env::home, ".cache" };
					dir = fmt::join(span, sys::sep::dir);
				}
				u = dir;
			}
			return view;
		}
	};

	struct DATA_DIRS : env::list
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
	};

	struct CONFIG_DIRS : env::list
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
	};

	std::string_view cached_dirs(std::string_view u)
	{
		static ini data;
		if (empty(data))
		{
			fmt::string_span const span { xdg::config_home, "user-dirs.dirs" };
			auto const path = fmt::join(span, sys::sep::dir);
			std::istream in(path);
			while (data.feed(in));
		}
		return data(u);
	}

	struct DESKTOP_DIR : env::view
	{
		constexpr auto var = "XDG_DESKTOP_DIR";

		operator std::string_view() const final
		{
			std::string_view u = sys::env::get(var);
			if (empty(u))
			{
				 u = cached_dirs()
			}
			return u;
		}
	};
}

namespace xdg
{
	env::view const& current_desktop = CURRENT_DESKTOP();
	env::view const& menu_prefix = MENU_PREFIX();
	env::view const& runtime_dir = RUNTIME_DIR();
	env::view const& data_home = DATA_HOME();
	env::view const& config_home = CONFIG_HOME();
	env::view const& cache_home = CACHE_HOME();
	env::list const& data_dirs = DATA_DIRS();
	env::list const& config_dirs = CONFIG_DIRS();
	env::view const& desktop_dir = DESKTOP_DIR();
}

