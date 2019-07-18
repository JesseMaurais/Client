// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "xdg.hpp"
#include "fmt.hpp"
#include "ini.hpp"
#include "sys.hpp"
#include "dir.hpp"
#include "dbg.hpp"
#include <fstream>

#ifdef _WIN32
# include <shlobj.h>
# pragma comment(lib, "shell32.lib")
# pragma comment(lib, "ole32.lib")
#endif

namespace
{
	bool exists(fmt::string_view u)
	{
		assert(fmt::terminated(u));
		char const* path = u.data();
		int const check = sys::access(path, F_OK);
		return not sys::fail(check);
	}

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
				u = xdg::current_desktop;
				if (not empty(u))
				{
					static auto prefix = fmt::to_lower(u) + '-';
					u = prefix;
				}
			}
			return u;
		}

	} XDG_MENU_PREFIX;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			static fmt::string path;
			if (empty(path))
			{
				auto const menu = fmt::join(xdg::menu_prefix, "applications.menu");
				path = fmt::dir::join(xdg::config_home, "menus", menu);
				if (not exists(path))
				{
					fmt::string_view_span list = xdg::config_dirs;
					for (auto const dir : list)
					{
						path = fmt::dir::join(dir, menu);
						if (exists(path)) break;
						path.clear();
					}
				}
			}
			return path;
		}

	} XDG_APPLICATIONS_MENU;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			fmt::string_view u = sys::env::get("XDG_RUNTIME_DIR");
			if (empty(u))
			{
				static auto const path = fmt::dir::join(env::tmpdir, "run", env::user);
				u = path;
			}
			return u;
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
					s = fmt::dir::join(env::home, ".local", "share");
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
					s = fmt::dir::join(env::home, ".config");
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
					s = fmt::dir::join(env::home, ".cache");
				}
				u = s;
			}
			return u;
		}

	} XDG_CACHE_HOME;

	struct : env::list
	{
		operator fmt::string_view_span() const final
		{
			fmt::string_view u = sys::env::get("XDG_DATA_DIRS");
			if (empty(u))
			{
				if constexpr (sys::posix)
				{
					u = "/usr/local/share/:/usr/share/";
				}
				else
				if constexpr (sys::win32)
				{
					u = sys::env::get("ALLUSERSPROFILE");
				}
			}
			return fmt::path::split(u);
		}

	} XDG_DATA_DIRS;

	struct : env::list
	{
		operator fmt::string_view_span() const final
		{
			static fmt::string_view_vector t;
			fmt::string_view u = sys::env::get("XDG_CONFIG_DIRS");
			if (empty(u))
			{
				if constexpr (sys::posix)
				{
					u = "/etc/xdg";
				}
				else
				if constexpr (sys::win32)
				{
					static fmt::string s;
					if (empty(s))
					{
						auto const appdata = fmt::to_string(sys::env::get("APPDATA"));
						auto const local = fmt::to_string(sys::env::get("LOCALAPPDATA"));
						s = fmt::path::join(appdata, local);
					}
					u = s;
				}
			}
			t = fmt::path::split(u);
			return t;
		}

	} XDG_CONFIG_DIRS;

	constexpr auto Desktop = "Desktop";
	constexpr auto Documents = "Documents";
	constexpr auto Downloads = "Downloads";
	constexpr auto Music = "Music";
	constexpr auto Pictures = "Pictures";
	constexpr auto PublicShare = "Public";
	constexpr auto Templates = "Templates";
	constexpr auto Videos = "Videos";

	struct user_dir : env::view
	{
		operator fmt::string_view() const final
		{
			fmt::string_view u = sys::env::get(var);
			if (empty(u))
			{
				u = cached();
			}
			
			#ifdef _WIN32
			{
				if (empty(u))
				{
					static std::map<fmt::string_view, KNOWNFOLDERID> map =
					{
						{ Desktop, FOLDERID_Desktop },
						{ Documents, FOLDERID_Documents },
						{ Downloads, FOLDERID_Downloads },
						{ Music, FOLDERID_Music },
						{ Pictures, FOLDERID_Pictures },
						{ PublicShare, FOLDERID_Public },
						{ Templates, FOLDERID_Templates },
						{ Videos, FOLDERID_Videos },
					};
					struct scoped
					{
						~scoped() { if (p) CoTaskMemFree(p); }
						PWSTR p = nullptr;
					} ws;
					HRESULT const ok = SHGetKnownFolderPath
					(
						map.at(val),
						0,
						nullptr,
						&ws.p
					);
					if (S_OK == ok)
					{
						static std::string s;
						s = fmt::to_string(ws.p);
						u = s;
					}
				}
			}
			#endif
			
			if (empty(u))
			{
				static fmt::string s;
				s = fmt::dir::join(env::home, val);
				u = s;
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
				auto const path = fmt::dir::join(xdg::config_home, "user-dirs.dirs");
				std::ifstream in(path);
				fmt::string line;
				while (ini::getline(in, line))
				{
					constexpr char quote = '"';
					auto const first = line.find_first_not_of(quote);
					auto const second = line.find_first_of(quote, first);
					line = line.substr(first, second);
					auto pair = fmt::to_pair(line);
					pair.second = sys::env::value(fmt::to_string(pair.second));
					data.emplace(pair);
				}
			}
			fmt::string const name(var);
			auto const it = data.find(name);
			return data.end() == it ? "" : it->second;
		}
	};

	user_dir const XDG_DESKTOP_DIR("XDG_DESKTOP_DIR", Desktop);
	user_dir const XDG_DOCUMENTS_DIR("XDG_DOCUMENTS_DIR", Documents);
	user_dir const XDG_DOWNLOAD_DIR("XDG_DOWNLOAD_DIR", Downloads);
	user_dir const XDG_MUSIC_DIR("XDG_MUSIC_DIR", Music);
	user_dir const XDG_PICTURES_DIR("XDG_PICTURES_DIR", Pictures);
	user_dir const XDG_PUBLICSHARE_DIR("XDG_PUBLICSHARE_DIR", PublicShare);
	user_dir const XDG_TEMPLATES_DIR("XDG_TEMPLATES_DIR", Templates);
	user_dir const XDG_VIDEOS_DIR("XDG_VIDEOS_DIR", Videos);
}

namespace xdg
{
	env::view const& current_desktop = XDG_CURRENT_DESKTOP;
	env::view const& menu_prefix = XDG_MENU_PREFIX;
	env::view const& applications_menu = XDG_APPLICATIONS_MENU;
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

