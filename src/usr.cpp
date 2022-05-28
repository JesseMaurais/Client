// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "err.hpp"
#include "usr.hpp"
#include "env.hpp"
#include "opt.hpp"
#include "arg.hpp"
#include "dir.hpp"
#include "type.hpp"
#include <fstream>

#ifdef _WIN32
#include <shlobj.h>
#endif

namespace
{
	bool user_dirs()
	{
		fmt::string path;
		using namespace env::file;
		if (find(config(), regex("user-dirs.dirs") || to(path) || stop))
		{
			std::ifstream in(path);
			while (in >> env::opt::get);
			return in.good();
		}
		return failure;
	}
}

namespace env::usr
{
	fmt::view dir(fmt::view name)
	{
		auto u = fmt::tag::empty;

	#ifdef _WIN32

		using entry = std::pair<KNOWNFOLDERID, fmt::string::view>;

		static const std::map<fmt::string::view, entry> map =
		{
			{ "AccountPictures", entry{FOLDERID_AccountPictures, "%AppData%\\Microsoft\\Windows\\AccountPictures"}},
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
			const auto ok = SHGetKnownFolderPath
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

			if (u.empty())
			{
				u = path;
			}
		}

	#else

		static const std::map<fmt::view, fmt::pair> map =
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

		static const auto dirs [[maybe_unused]] =  user_dirs();

		if (auto it = map.find(name); map.end() != it)
		{
			auto [xdg, path] = it->second;

			u = env::opt::get(xdg);
			if (u.empty())
			{
				u = path;
			}
		}

	#endif

		if (u.empty())
		{
			u = env::get(name);
		}

		u = fmt::trim(u, fmt::tag::quote);
		return env::echo(u);
	}

	fmt::view current_desktop()
	{
		auto u = env::get("XDG_CURRENT_DESKTOP");
		if (u.empty())
		{
			u = env::session();
		}
		return u;
	}

	fmt::view menu_prefix()
	{
		auto u = env::get("XDG_MENU_PREFIX");
		if (u.empty())
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

	fmt::view applications_menu()
	{
		thread_local fmt::string path;
		if (path.empty())
		{
			const fmt::view prefix = menu_prefix();
			const fmt::view basename = "applications.menu";
			fmt::vector parts { prefix, basename };
			const auto file = fmt::join(parts, "-");
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

	fmt::view run_dir()
	{
		thread_local fmt::string path;
		auto u = dir("Runtime");
		if (u.empty())
		{
			const auto temp = env::temp();
			const auto user = env::user();
			static auto const path = fmt::dir::join({temp, "run", user});
			u = path;
		}
		return u;
	}

	fmt::view data_home()
	{
		auto u = env::get("XDG_DATA_HOME");
		if (u.empty())
		{
			static fmt::string s;
			if (s.empty())
			{
				const auto home = env::home();
				s = fmt::dir::join({home, ".local", "share"});
			}
			u = s;
		}
		return u;
	}

	fmt::view config_home()
	{
		auto u = env::get("XDG_CONFIG_HOME");
		if (u.empty())
		{
			static fmt::string s;
			if (s.empty())
			{
				s = fmt::dir::join({env::home(), ".config"});
			}
			u = s;
		}
		return u;
	}

	fmt::view cache_home()
	{
		auto u = env::get("XDG_CACHE_HOME");
		if (u.empty())
		{
			static fmt::string s;
			if (s.empty())
			{
				s = fmt::dir::join({env::home(), ".cache"});
			}
			u = s;
		}
		return u;
	}

	fmt::span data_dirs()
	{
		static fmt::vector t;
		auto u = env::get("XDG_DATA_DIRS");
		if (u.empty())
		{
			#ifdef _WIN32
			{
				u = env::get("AllUsersProfile");
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

	fmt::span config_dirs()
	{
		auto u = env::get("XDG_CONFIG_DIRS");
		if (u.empty())
		{
			#ifdef _WIN32
			{
				static fmt::string s;
				if (s.empty())
				{
					const auto appdata = dir("AppData");
					const auto local = dir("LocalAppData");
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

		static fmt::vector buf;
		buf = fmt::path::split(u);
		return buf;
	}
}

#ifdef test_unit
namespace
{
	struct head : fmt::memory<head>
	{
		fmt::view group;
		head(fmt::view g)
		: group(g)
		{ }
	};

	fmt::output operator<<(fmt::output out, head::cref obj)
	{
		return out << '[' << obj.group << ']' << fmt::tag::eol;
	}

	struct key : fmt::memory<key>
	{
		fmt::pair entry;
		key(fmt::view key, fmt::view value)
		: entry(key, value)
		{ }
	};

	fmt::output operator<<(fmt::output out, key::cref obj)
	{
		return out << obj.entry.first << '=' << obj.entry.second << fmt::tag::eol;
	}
}

test_unit(usr)
{
	std::ofstream out { ".ini" };

	out	<< head("Fake Environment")
		<< key("home", env::home())
		<< key("user", env::user())
		<< key("root", env::root())
		<< key("domain", env::domain())
		<< key("host", env::host())
		<< key("pwd", env::pwd())
		<< key("lang", env::lang())
		<< key("shell", env::shell())
		<< key("tmpdir", env::temp())
		<< key("rootdir", env::base())
		<< key("session", env::session())
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