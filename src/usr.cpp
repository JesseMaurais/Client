// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "usr.hpp"
#include "fmt.hpp"
#include "ini.hpp"
#include "opt.hpp"
#include "sys.hpp"
#include "dir.hpp"
#include "err.hpp"
#include <fstream>

#ifdef _WIN32
# include <shlobj.h>
# pragma comment(lib, "shell32.lib")
# pragma comment(lib, "ole32.lib")
#endif

namespace
{
	struct : env::view
	{
		operator fmt::string_view() const final
		{
			auto u = env::sys::get("XDG_CURRENT_DESKTOP");
			if (empty(u))
			{
				u = env::desktop;
			}
			return u;
		}

	} CURRENT_DESKTOP;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			auto u = env::sys::get("XDG_MENU_PREFIX");
			if (empty(u))
			{
				u = env::usr::current_desktop;
				if (not empty(u))
				{
					static auto prefix = fmt::to_lower(u) + '-';
					u = prefix;
				}
			}
			return u;
		}

	} MENU_PREFIX;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			static fmt::string path;
			if (empty(path))
			{
				constexpr auto base = "applications.menu";
				auto const menu = fmt::join({env::usr::menu_prefix, base});
				path = fmt::dir::join(env::usr::config_home, "menus", menu);
				if (sys::path::fail(path))
				{
					fmt::string_view_span span = env::usr::config_dirs;
					for (auto const dir : span)
					{
						path = fmt::dir::join(dir, menu);
						if (not sys::dir::fail(path))
						{
							break;
						}
						path.clear();
					}
				}
			}
			return path;
		}

	} APPLICATIONS_MENU;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			auto u = env::sys::get("XDG_RUNTIME_DIR");
			if (empty(u))
			{
				static auto const path = fmt::dir::join(env::tmpdir, "run", env::user);
				u = path;
			}
			return u;
		}

	} RUNTIME_DIR;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			auto u = env::sys::get("XDG_DATA_HOME");
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

	} DATA_HOME;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			auto u = env::sys::get("XDG_CONFIG_HOME");
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

	} CONFIG_HOME;

	struct : env::view
	{
		operator fmt::string_view() const final
		{
			auto u = env::sys::get("XDG_CACHE_HOME");
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

	} CACHE_HOME;

	struct : env::span
	{
		operator fmt::string_view_span() const final
		{
			auto u = env::sys::get("XDG_DATA_DIRS");
			if (empty(u))
			{
				#ifdef _WIN32
				{
					u = env::sys::get("ALLUSERSPROFILE");
				}
				#else
				{
					u = "/usr/local/share/:/usr/share/";
				}
				#endif
			}
			return fmt::path::split(u);
		}

	} DATA_DIRS;

	struct : env::span
	{
		operator fmt::string_view_span() const final
		{
			static fmt::string_view_vector t;
			auto u = env::sys::get("XDG_CONFIG_DIRS");
			if (empty(u))
			{
				#ifdef _WIN32
				{
					static fmt::string s;
					if (empty(s))
					{
						auto const appdata = fmt::to_string(env::sys::get("APPDATA"));
						auto const local = fmt::to_string(env::sys::get("LOCALAPPDATA"));
						s = fmt::path::join(appdata, local);
					}
					u = s;
				}
				#else
				{
					u = "/etc/xdg";
				}
				#endif
			}
			t = fmt::path::split(u);
			return t;
		}

	} CONFIG_DIRS;

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
			auto u = env::sys::get(var);
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

					PWSTR pws = nullptr;
					auto const id = map.at(val);
					auto const ok = SHGetKnownFolderPath
					(
						map.at(val),
						0,
						nullptr,
						&pws
					);

					if (S_OK == ok)
					{
						static fmt::string s;
						s = fmt::to_string(pws);
						u = s;
					}

					if (nullptr != pws)
					{
						CoTaskMemFree(pws);
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
			auto u = env::opt::get(var);
			if (empty(u))
			{
				constexpr auto base = "user-dirs.dirs";
				auto const path = fmt::dir::join(env::usr::config_home, base);
				std::ifstream in(path);
				fmt::string line;
				while (doc::ini::getline(in, line))
				{
					constexpr char quote = '"';
					auto const first = line.find_first_not_of(quote);
					auto const second = line.find_first_of(quote, first);
					line = line.substr(first, second);
					auto entry = fmt::entry(line);
					auto value = env::sys::value(entry.second);
					env::opt::set(entry.first, value);
				}
				u = env::opt::get(var);
			}
			return u;
		}
	};

	user_dir const DESKTOP_DIR("XDG_DESKTOP_DIR", Desktop);
	user_dir const DOCUMENTS_DIR("XDG_DOCUMENTS_DIR", Documents);
	user_dir const DOWNLOAD_DIR("XDG_DOWNLOAD_DIR", Downloads);
	user_dir const MUSIC_DIR("XDG_MUSIC_DIR", Music);
	user_dir const PICTURES_DIR("XDG_PICTURES_DIR", Pictures);
	user_dir const PUBLICSHARE_DIR("XDG_PUBLICSHARE_DIR", PublicShare);
	user_dir const TEMPLATES_DIR("XDG_TEMPLATES_DIR", Templates);
	user_dir const VIDEOS_DIR("XDG_VIDEOS_DIR", Videos);
}

namespace env::usr
{
	env::view const& current_desktop = CURRENT_DESKTOP;
	env::view const& menu_prefix = MENU_PREFIX;
	env::view const& applications_menu = APPLICATIONS_MENU;
	env::view const& runtime_dir = RUNTIME_DIR;
	env::view const& data_home = DATA_HOME;
	env::view const& config_home = CONFIG_HOME;
	env::view const& cache_home = CACHE_HOME;
	env::span const& data_dirs = DATA_DIRS;
	env::span const& config_dirs = CONFIG_DIRS;
	env::view const& desktop_dir = DESKTOP_DIR;
	env::view const& documents_dir = DOCUMENTS_DIR;
	env::view const& download_dir = DOWNLOAD_DIR;
	env::view const& music_dir = MUSIC_DIR;
	env::view const& pictures_dir = PICTURES_DIR;
	env::view const& publicshare_dir = PUBLICSHARE_DIR;
	env::view const& templates_dir = TEMPLATES_DIR;
	env::view const& videos_dir = VIDEOS_DIR;
}
