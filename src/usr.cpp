// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "usr.hpp"
#include "env.hpp"
#include "opt.hpp"
#include "dir.hpp"
#include "type.hpp"
#include "mode.hpp"
#include "err.hpp"

#include <iostream>

#ifdef _WIN32
#include <shlobj.h>
#endif

namespace env::usr
{
	fmt::string::view dir(fmt::string::view name)
	{
		fmt::string::view u = fmt::empty;
		
	#ifdef _WIN32

		using entry = std::pair<KNOWNFOLDERID, fmt::string::view>;

		static std::map<fmt::string::view, entry> const map =
		{
			{ "AccountPictures", entry{FOLDERID_AccountPictures, "%AppData%\\Microsoft\\Winodws\\AccountPictures"}},
			{ "AdminTools", entry{FOLDERID_AdminTools, "%AppData%\\Microsoft\\Windows\\Start Menu\\Programs\\Administrative Tools"}},
			{ "AppDataDesktop", entry{FOLDERID_AppDataDesktop, "%LocalAppData%\\Desktop"}},
			{ "AppDataDocuments", entry{FOLDERID_AppDataDocuments, "%LocalAppData%\\Documents"}},
			{ "AppDataFavorites", entry{FOLDERID_AppDataDocuments, "%LocalAppData%\\Favorites"}},
			{ "AppDataProgramData", entry{FOLDERID_AppDataProgramData, "%LocalAppData%\\ProgramData"}},
			{ "ApplicationShortcuts", entry{FOLDERID_ApplicationShortcuts, "%LocalAppData%\\Microsoft\\Windows\\Application Shortcuts"}},
			{ "CDBurning", entry{FOLDERID_CDBurning, "%LocalAppData%\\Microsoft\\Windows\\Burn\\Burn"}},
			{ "CommonAdminTools", entry{FOLDERID_CommonAdminTools, "%AllUsersProfile%\\Start Menu\\Programs\\Administrative Tools"}},
			{ "CommonPrograms", entry{FOLDERID_CommonStartMenu, "%AllUsersProfile%\\Start Menu\\Programs"}},
			{ "CommonStartMenu", entry{FOLDERID_CommonStartMenu, "%AllUsersProfile%\\Start Menu"}},
			{ "CommonStartup", entry{FOLDERID_CommonStartup, "%AllUsersProfile%\\Start Menu\\StartUp"}},
//			{ "CommonTemplates" {FOLDERID_CommonTemplates, "%AllUsersProfile%\\Templates"}},
			{ "LocalAppData", entry{FOLDERID_LocalAppData, "%UserProfile%\\AppData\\Local"}},
			{ "Camera", entry{FOLDERID_CameraRoll, "%UserProfile%\\Pictures\\Camera Roll"}},
			{ "Contacts", entry{FOLDERID_Contacts, "%UserProfile%\\Contacts"}},
			{ "Cookies", entry{FOLDERID_Cookies, "%UserProfile%\\Cookies"}},
			{ "Desktop", entry{FOLDERID_Desktop, "%UserProfile%\\Desktop"}},
			{ "Documents", entry{FOLDERID_Documents, "%UserProfile%\\Documents"}},
			{ "Downloads", entry{FOLDERID_Downloads, "%UserProfile%\\Downloads"}},
			{ "Favorites", entry{FOLDERID_Favorites, "%UserProfile%\\Favorites"}},
			{ "Fonts", entry{FOLDERID_Fonts, "%WinDir%\\Fonts" }},
			{ "History", entry{FOLDERID_History, "%UserProfile%\\Local Settings\\History"}},
			{ "InternetCache", entry{FOLDERID_InternetCache, "%LocalAppData%\\Local Settings\\Temporary"}},
			{ "Links", entry{FOLDERID_Links, "%UserProfile%\\Links"}},
			{ "Objects3D", entry{FOLDERID_Objects3D, "%UserProfile%\\Objects3D"}},
			{ "Resources", entry{FOLDERID_LocalizedResourcesDir, "%WinDir%\\Resources"}},
			{ "Music", entry{FOLDERID_Music, "%UserProfile%\\Music;%UserProfile%\\My Documents\\My Music"}},
			{ "Pictures", entry{FOLDERID_Pictures, "%UserProfile%\\Pictures;%UserProfile%\\My Documents\\My Pictures"}},
			{ "Profile", entry{FOLDERID_Profile, "%UserProfile%"}},
			{ "ProgramData", entry{FOLDERID_ProgramData, "%AllUsersProfile%\\Application Data;%AllUsersProfile%;%ProgramData%;%SystemDrive%\\ProgramData"}},
			{ "ProgramFilesCommon", entry{FOLDERID_ProgramFilesCommon, "%ProgramFiles%\\Common Files"}},
			{ "ProgramFiles", entry{FOLDERID_ProgramFiles, "%ProgramFiles%;%SystemDrive%\\Program Files"}},
			{ "Programs", entry{FOLDERID_Programs, "%UserProfile%\\Start Menu\\Programs"}},
			{ "Public", entry{FOLDERID_Public, "%Public%;%SystemDrive%\\Users\\Public"}},
			{ "PublicDesktop", entry{FOLDERID_PublicDesktop, "%Public%\\Desktop;%AllUsersProfile%\\Desktop"}},
			{ "PublicDocuments", entry{FOLDERID_PublicDocuments, "%Public%\\Documents;%AllUsersProfile%\\Documents"}},
			{ "PublicDownloads", entry{FOLDERID_PublicDownloads, "%Public%\\Downloads"}},
			{ "Screenshot", entry{FOLDERID_Screenshots, "%UserProfile%\\Pictures\\Screenshots"}},
			{ "StartMenu", entry{FOLDERID_StartMenu, "%UserProfile%\\Start Menu"}},
			{ "Startup", entry{FOLDERID_Startup, "%UserProfile%\\Start Menu\\Programs\\StartUp"}},
			{ "System", entry{FOLDERID_System, "%WinDir%\\System32"}},
			{ "Templates", entry{FOLDERID_Templates, "%UserProfile%\\Templates"}},
			{ "UserProfiles", entry{FOLDERID_UserProfiles, "%SystemDrive%\\Users"}},
			{ "UserProgramFiles", entry{FOLDERID_UserProgramFiles, "%LocalAppData%\\Programs"}},
			{ "UserProgramFilesCommon", entry{FOLDERID_UserProgramFilesCommon, "%LocalAppData%\\Programs\\Common"}},
			{ "Videos", entry{FOLDERID_Videos, "%UserProfile%\\Videos;%UserProfile%\\My Documents\\My Videos"}},
			{ "Windows", entry{FOLDERID_Windows, "%WinDir%"}}
		};

		if (auto it = map.find(name); map.end() != it)
		{
			auto [id, path] = it->second;

			PWSTR pws = nullptr;
			auto const ok = SHGetKnownFolderPath
			(
				id, KF_FLAG_DEFAULT, nullptr, &pws
			);

			if (S_OK == ok)
			{
				thread_local fmt::string buf;
				buf = fmt::to_string(pws);
				u = buf;
			}

			if (nullptr != pws)
			{
				CoTaskMemFree(pws);
			}

			if (empty(u))
			{
				u = path;
			}
		}

	#else // POSIX

		using entry = std::initializer_list<fmt::string::view>;

		static std::map<fmt::string::view, entry> const map =
		{
			{"Cache-Home", {"XDG_CACHE_HOME", "$HOME/.cache"}},
			{"Config-Dirs", {"XDG_CONFIG_DIRS", "/etc/xdg"}},
			{"Config-Home", {"XDG_CONFIG_HOME", "$HOME/.config"}},
			{"Data-Dirs", {"XDG_DATA_DIRS", "/usr/local/share:/usr/share"}},
			{"Data-Home", {"XDG_DATA_HOME", "$HOME/.local/share"}},
			{"Desktop", {"XDG_DESKTOP_DIR", "$HOME/Desktop"}},
			{"Documents", {"XDG_DOCUMENTS_DIR", "$HOME/Documents"}},
			{"Downloads", {"XDG_DOWNLOAD_DIR", "$HOME/Downloads"}},
			{"Music", {"XDG_MUSIC_DIR", "$HOME/Music"}},
			{"Runtime", {"XDG_RUNTIME_DIR", "$TMPDIR:$TEMP:$TMP"}},
			{"Pictures", {"XDG_PICTURES_DIR", "$HOME/Pictures"}},
			{"Public", {"XDG_PUBLICSHARE_DIR", "$HOME/Public"}},
			{"State-Home", {"XDG_STATE_HOME", "$HOME/.local/state"}},
			{"Templates", {"XDG_TEMPLATES_DIR", "$HOME/Templates"}},
			{"Videos", {"XDG_VIDEOS_DIR", "$HOME/Videos"}},
		};

		if (auto it = map.find(name); map.end() != it)
		{
			auto [xdg, path] = it->second;

			u = env::get(xdg);
			if (empty(u))
			{
				u = path;
			}
		}

	#endif

		if (empty(u))
		{
			u = env::get(name);
		}

		return env::echo(u);
	}

	fmt::string::view::span dirs(fmt::string::view name)
	{
		thread_local fmt::string::view::vector buf;
		buf.clear();

		if (auto paths = dir(name); not empty(paths))
		{
			buf = fmt::path::split(paths);
		}

		for (auto it = buf.begin(); it != buf.end(); ++it)
		{
			if (auto path = *it; env::file::fail(path))
			{
				it = buf.erase(it);
			}
		}
		return buf;
	}

	fmt::string::view current_desktop()
	{
		auto u = env::get("XDG_CURRENT_DESKTOP");
		if (empty(u))
		{
			u = env::var::session();
		}
		return u;
	}

	fmt::string::view menu_prefix()
	{
		auto u = env::get("XDG_MENU_PREFIX");
		if (empty(u))
		{
			u = current_desktop();
		}
		else
		if (u.back() == '-')
		{
			u = u.substr(0, u.size() - 1);
		}
		return u;
	}

	fmt::string::view applications_menu()
	{
		thread_local fmt::string path;
		if (empty(path))
		{
			const auto prefix = menu_prefix();
			const auto file = fmt::join({prefix, "applications.menu"}, "-");
			const auto found = env::file::find(env::file::config(), [&](auto dir)
			{
				path = fmt::dir::join({dir, "menus", file});
				return not env::file::fail(path);
			});

			if (not found)
			{
				path.clear();
			}
		}
		return path;
	}

	fmt::string::view run_dir()
	{
		thread_local fmt::string path;
		auto u = dir("Runtime");
		if (empty(u))
		{
			auto const temp = env::var::temp();
			auto const user = env::var::user();
			static auto const path = fmt::dir::join({temp, "run", user});
			u = path;
		}
		return u;
	}

	fmt::string::view data_home()
	{
		auto u = env::get("XDG_DATA_HOME");
		if (empty(u))
		{
			static fmt::string s;
			if (empty(s))
			{
				auto const home = env::var::home();
				s = fmt::dir::join({home, ".local", "share"});
			}
			u = s;
		}
		return u;
	}

	fmt::string::view config_home()
	{
		auto u = env::get("XDG_CONFIG_HOME");
		if (empty(u))
		{
			static fmt::string s;
			if (empty(s))
			{
				s = fmt::dir::join({env::var::home(), ".config"});
			}
			u = s;
		}
		return u;
	}

	fmt::string::view cache_home()
	{
		auto u = env::get("XDG_CACHE_HOME");
		if (empty(u))
		{
			static fmt::string s;
			if (empty(s))
			{
				s = fmt::dir::join({env::var::home(), ".cache"});
			}
			u = s;
		}
		return u;
	}

	fmt::string::view::span data_dirs()
	{
		static fmt::string::view::vector t;
		auto u = env::get("XDG_DATA_DIRS");
		if (empty(u))
		{
			#ifdef _WIN32
			{
				u = env::get("ALLUSERSPROFILE");
			}
			#else
			{
				u = "/usr/local/share/:/usr/share/";
			}
			#endif
		}
		t = fmt::path::split(u);
		return t;
	}

	fmt::string::view::span config_dirs()
	{
		auto u = env::get("XDG_CONFIG_DIRS");
		if (empty(u))
		{
			#ifdef _WIN32
			{
				static fmt::string s;
				if (empty(s))
				{
					auto const appdata = dir("AppData");
					auto const local = dir("LocalAppData");
					s = fmt::path::join({appdata, local});
				}
				u = s;
			}
			#else
			{
				u = "/etc/xdg";
			}
			#endif
		}

		static fmt::string::view::vector buf;
		buf = fmt::path::split(u);
		return buf;
	}

	using namespace std::literals;

}

#ifdef test_unit
 namespace
{
	struct head : fmt::layout<head>
	{
		fmt::string::view group;

		head(fmt::string::view g)
			: group(g)
		{ }
	};

	fmt::string::out::ref operator<<(fmt::string::out::ref out, head::cref obj)
	{
		return out << '[' << obj.group << ']' << fmt::eol;
	}

	struct key : fmt::layout<key>
	{
		fmt::string::view::pair entry;

		key(fmt::string::view key, fmt::string::view value)
			: entry(key, value)
		{ }
	};

	fmt::string::out::ref operator<<(fmt::string::out::ref out, key::cref obj)
	{
		return out << obj.entry.first << '=' << obj.entry.second << fmt::eol;
	}
}

test_unit(usr)
{
	std::ofstream out { ".ini" };

	out	<< head("Fake Environment")
		<< key("home", env::var::home())
		<< key("user", env::var::user())
		<< key("root", env::var::root())
		<< key("domain", env::var::domain())
		<< key("host", env::var::host())
		<< key("pwd", env::var::pwd())
		<< key("lang", env::var::lang())
		<< key("shell", env::var::shell())
		<< key("tmpdir", env::var::temp())
		<< key("rootdir", env::var::base())
		<< key("session", env::var::session())
		<< std::endl;

	out	<< head("Desktop")
		<< key("runtime-dir", env::usr::run_dir())
		<< key("data-home", env::usr::data_home())
		<< key("config-home", env::usr::config_home())
		<< key("cache-home", env::usr::cache_home())
		<< key("data-dirs", fmt::path::join(env::usr::data_dirs()))
		<< key("config-dirs", fmt::path::join(env::usr::config_dirs()))
		<< std::endl;

	out	<< head("User Directories")
		<< key("desktop-dir", env::usr::desktop_dir())
		<< key("documents-dir", env::usr::documents_dir())
		<< key("download-dir", env::usr::download_dir())
		<< key("music-dir", env::usr::music_dir())
		<< key("pictures-dir", env::usr::pictures_dir())
		<< key("publicshare-dir", env::usr::public_dir())
		<< key("template-dir", env::usr::templates_dir())
		<< key("videos-dir", env::usr::videos_dir())
		<< std::endl;

	out	<< head("Application Options")
		<< key("name", env::opt::application())
		<< key("program", env::opt::program())
		<< key("command-line", fmt::join(env::opt::arguments(), " "))
		<< key("config", env::opt::config())
		<< std::endl;

	out << env::opt::put << std::endl;
}
#endif