#include "xdg.hpp"
#include "fmt.hpp"
#include "ini.hpp"
#include "sys.hpp"
#include <fstream>

namespace
{
	struct : env::view
	{
		operator std::string_view() const final
		{
			std::string_view u = sys::env::get("XDG_CURRENT_DESKTOP");
			if (empty(u))
			{
				u = env::desktop;
			}
			return u;
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
				static std::string s;
				if (empty(s))
				{
					fmt::span_view const p { env::home, ".local", "share" };
					s = fmt::join(p, sys::sep::dir);
				}
				u = s;
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
				static std::string s;
				if (empty(s))
				{
					fmt::span_view const p { env::home, ".config" };
					s = fmt::join(p, sys::sep::dir);
				}
				u = s;
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
				static std::string s;
				if (empty(s))
				{
					fmt::span_view const p { env::home, ".cache" };
					s = fmt::join(p, sys::sep::dir);
				}
				u = s;
			}
			return u;
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
					u = sys::env::get("ALLUSERSPROFILE");
				}
			}
			return fmt::split(u, sys::sep::path);
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
					static std::string s;
					if (empty(s))
					{
						u = sys::env::get("APPDATA");
						s.append(u.data(), u.size());
						
						s += sys::sep::path;

						u = sys::env::get("LOCALAPPDATA");
						s.append(u.data(), u.size());
					}
					u = s;
				}
			}
			return fmt::split(u, sys::sep::path);
		}

	} XDG_CONFIG_DIRS;

	struct cached_dir : env::view
	{
		operator std::string_view() const final
		{
			std::string_view u = sys::env::get(var);
			if (empty(u))
			{
				u = cached();
			}
			if (empty(u))
			{
				u = env::home;
			}
			return u;
		}

		cached_dir(char const *xdgvar)
		: var(xdgvar)
		{ }

	private:

		char const *var;

		std::string_view cached() const
		{
			static ini::entry data;
			if (empty(data))
			{
				fmt::span_view const p { xdg::config_home, "user-dirs.dirs" };
				auto const path = fmt::join(p, sys::sep::dir);
				std::ifstream in(path);
				std::string s;
				while (ini::getline(in, s))
				{
					fmt::pair p = fmt::key_value(s);
					p.second = sys::env::eval(p.second);
					data.emplace(p);
				}
			}
			std::string const s(var);
			return data.at(s);
		}
	};

	cached_dir const XDG_DESKTOP_DIR("XDG_DESKTOP_DIR");
	cached_dir const XDG_DOCUMENTS_DIR("XDG_DOCUMENTS_DIR");
	cached_dir const XDG_DOWNLOAD_DIR("XDG_DOWNLOAD_DIR");
	cached_dir const XDG_MUSIC_DIR("XDG_MUSIC_DIR");
	cached_dir const XDG_PICTURES_DIR("XDG_PICTURES_DIR");
	cached_dir const XDG_PUBLICSHARE_DIR("XDG_PUBLICSHARE_DIR");
	cached_dir const XDG_TEMPLATES_DIR("XDG_TEMPLATES_DIR");
	cached_dir const XDG_VIDEOS_DIR("XDG_VIDEOS_DIR");
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
	env::view const& documents_dir = XDG_DOCUMENTS_DIR;
	env::view const& download_dir = XDG_DOWNLOAD_DIR;
	env::view const& music_dir = XDG_MUSIC_DIR;
	env::view const& pictures_dir = XDG_PICTURES_DIR;
	env::view const& publicshare_dir = XDG_PUBLICSHARE_DIR;
	env::view const& templates_dir = XDG_TEMPLATES_DIR;
	env::view const& videos_dir = XDG_VIDEOS_DIR;
}

