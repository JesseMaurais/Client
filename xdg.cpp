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

	struct DATA_HOME : env::view
	{
		operator std::string_view() const final
		{
			constexpr auto var = "XDG_DATA_HOME";
			std::string_view view = sys::env::get(var);
			if (view.empty())
			{
				std::vector<std::string_view> parts { env::home, ".local", "share" };
				std::string const dir = fmt::join(parts, sys::sep::dir);
				if (sys::env::set(var, dir))
				{
					sys::perror(var, dir);
				}
				view = sys::env::get(var);
			}
			return view;
		}
	};

	struct CONFIG_HOME : env::view
	{
		operator std::string_view() const final
		{
			constexpr auto var = "XDG_CONFIG_HOME";
			std::string_view view = sys::env::get(var);
			if (view.empty())
			{
				std::vector<std::string_view> parts { env::home, ".config" };
				std::string const dir = fmt::join(parts, sys::sep::dir);
				if (sys::env::set(var, dir))
				{
					sys::perror(var, dir);
				}
				view = sys::env::get(var);
			}
			return view;
		}
	};

	struct CACHE_HOME : env::view
	{
		operator std::string_view() const final
		{
			constexpr auto var = "XDG_CACHE_HOME";
			std::string_view view = sys::env::get(var);
			if (view.empty())
			{
				std::vector<std::string_view> parts { env::home, ".cache" };
				std::string const dir = fmt::join(parts, sys::sep::dir);
				if (sys::env::set(var, dir))
				{
					sys::perror(var, dir);
				}
				view = sys::env::get(var);
			}
			return view;
		}
	};
}

namespace xdg
{
	env::view const& current_desktop = CURRENT_DESKTOP();
	env::view const& data_home = DATA_HOME();
	env::view const& config_home = CONFIG_HOME();
	env::view const& cache_home = CACHE_HOME();
}

