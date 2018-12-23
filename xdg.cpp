#include "xdg.hpp"
#include "fmt.hpp"
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
			std::string_view view = sys::env::get("XDG_MENU_PREFIX");
			if (empty(view))
			{
				static std::string prefix;
				if (empty(prefix))
				{
					prefix = to_lower(xdg::current_desktop) + '-';
				}
				view = prefix;
			}
			return view;
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
			std::string_view view = sys::env::get("XDG_DATA_HOME");
			if (empty(view))
			{
				static std::string dir;
				if (empty(dir))
				{
					fmt::string_span folders { env::home, ".local", "share" };
					dir = fmt::join(folders, sys::sep::dir);
				}
				view = dir;
			}
			return view;
		}
	};

	struct CONFIG_HOME : env::view
	{
		operator std::string_view() const final
		{
			std::string_view view = sys::env::get("XDG_CONFIG_HOME");
			if (empty(view))
			{
				static std::string dir;
				if (empty(dir))
				{
					fmt::string_span folders { env::home, ".config" };
					dir = fmt::join(folders, sys::sep::dir);
				}
				view = dir;
			}
			return view;
		}
	};

	struct CACHE_HOME : env::view
	{
		operator std::string_view() const final
		{
			std::string_view view = sys::env::get("XDG_CACHE_HOME");
			if (empty(view))
			{
				static std::string dir;
				if (empty(dir))
				{
					fmt::string_span folders { env::home, ".cache" };
					dir = fmt::join(folders, sys::sep::dir);
				}
				view = dir;
			}
			return view;
		}
	};

	struct DATA_DIRS : env::list
	{
		operator std::vector<std::string_view>() const final
		{
			std::string_view view = sys::env::get("XDG_DATA_DIRS");
			if (empty(view))
			{
				if constexpr (sys::POSIX)
				{
					static auto const dir = "/usr/local/share/:/usr/share/";
					view = dir;
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
					view = dir;
				}
			}
			return fmt::split(view, sys::sep::dir);
		}
	};

	struct CONFIG_DIRS : env::list
	{
		operator std::vector<std::string_view>() const final
		{
			std::string_view view = sys::env::get("XDG_CONFIG_DIRS");
			if (empty(view))
			{
				if constexpr (sys::POSIX)
				{
					static auto const dir = "/etc/xdg";
					view = dir;
				}
				else
				if constexpr (sys::WIN32)
				{
					static std::string dir = sys::env::get("ALLUSERSPROFILE");
					view = dir;
				}
			}
			return fmt::split(view, sys::sep::dir);
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
}

