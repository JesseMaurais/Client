#include "xdg.hpp"
#include "fmt.hpp"
#include "ini.hpp"
#include "sys.hpp"
#include <fstream>

namespace
{
	struct : env::view
	{
		operator fmt::string_view() const final
		{
			fmt::string_view u = sys::env::get("XDG_CURRENT_DESKTOP");
			if (empty(u))
			{
				u = env::desktop;
			}
			return u;
		}

	} XDG_CURRENT_DESKTOP;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			fmt::string_view u = sys::env::get("XDG_MENU_PREFIX");
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
		operator fmt::string_view() const final
		{
			return sys::env::get("XDG_RUNTIME_DIR");
		}

	} XDG_RUNTIME_DIR;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			fmt::string_view u = sys::env::get("XDG_DATA_HOME");
			if (empty(u))
			{
				static fmt::string s;
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
		operator fmt::string_view() const final
		{
			fmt::string_view u = sys::env::get("XDG_CONFIG_HOME");
			if (empty(u))
			{
				static fmt::string s;
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
		operator fmt::string_view() const final
		{
			fmt::string_view u = sys::env::get("XDG_CACHE_HOME");
			if (empty(u))
			{
				static fmt::string s;
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
		operator fmt::span_view() const final
		{
			fmt::string_view u = sys::env::get("XDG_DATA_DIRS");
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
		operator fmt::span_view() const final
		{
			fmt::string_view u = sys::env::get("XDG_CONFIG_DIRS");
			if (empty(u))
			{
				if constexpr (sys::POSIX)
				{
					u = "/etc/xdg";
				}
				else
				if constexpr (sys::WIN32)
				{
					static fmt::string s;
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

	struct user_dir : env::view
	{
		operator fmt::string_view() const final
		{
			fmt::string_view u = sys::env::get(var);
			if (empty(u))
			{
				u = cached();
			}
			return u;
		}

		user_dir(char const *variable, char const *value)
		: var(variable)
		, val(value)
		{ }

	private:

		char const *var, *val;

		fmt::string_view cached() const
		{
			static ini::entry data;
			if (empty(data))
			{
				fmt::span_view const p { xdg::config_home, "user-dirs.dirs" };
				auto const path = fmt::join(p, sys::sep::dir);
				std::ifstream in(path);
				fmt::string s;
				while (ini::getline(in, s))
				{
					fmt::pair p = fmt::key_value(s);
					p.second = sys::env::eval(p.second);
					data.emplace(p);
				}
			}
			fmt::string const name(var);
			auto it = data.find(name);
			if (data.end() == it)
			{
				fmt::span_view const p { env::home, val };
				fmt::string const dir = fmt::join(p, sys::sep::dir);
				it = data.emplace(name, dir).first;
			}
			return it->second;
		}
	};

	user_dir const XDG_DESKTOP_DIR("XDG_DESKTOP_DIR", "Desktop");
	user_dir const XDG_DOCUMENTS_DIR("XDG_DOCUMENTS_DIR", "Documents");
	user_dir const XDG_DOWNLOAD_DIR("XDG_DOWNLOAD_DIR", "Downloads");
	user_dir const XDG_MUSIC_DIR("XDG_MUSIC_DIR", "Music");
	user_dir const XDG_PICTURES_DIR("XDG_PICTURES_DIR", "Pictures");
	user_dir const XDG_PUBLICSHARE_DIR("XDG_PUBLICSHARE_DIR", "Public");
	user_dir const XDG_TEMPLATES_DIR("XDG_TEMPLATES_DIR", "Templates");
	user_dir const XDG_VIDEOS_DIR("XDG_VIDEOS_DIR", "Videos");
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

