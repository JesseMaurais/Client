// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "usr.hpp"
#include "env.hpp"
#include "type.hpp"
#include "dir.hpp"
#include "err.hpp"

#ifdef _WIN32
#include <shlobj.h>
#ifdef _MSC_VER
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")
#endif // _MSC_VER
#endif // _WIN32

namespace env::usr
{
	fmt::string::view dir(fmt::string::view path)
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

		auto it = map.find(path);
		if (map.end() != it)
		{
			PWSTR pws = nullptr;
			auto const ok = SHGetKnownFolderPath
			(
				it->second.first, KF_FLAG_DEFAULT, nullptr, &pws
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
		}

	#else // POSIX

		static std::map
		<
			fmt::string::view, std::initializer_list<fmt::string::view>
		> 
		const map =
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

		auto it = map.find(path);
		if (map.end() != it)
		{
			u = env::get(it->first);
		}

	#endif

		if (empty(u))
		{
			if (auto v = it->second.second; not empty(v))
			{
				u = env::echo(v);
			}
		}
		return u;
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
			if (not u.empty())
			{
				static auto prefix = fmt::to_lower(u) + '-';
				u = prefix;
			}
		}
		return u;
	}

	fmt::string::view applications_menu()
	{
		static fmt::string path;
		if (empty(path))
		{
			constexpr auto base = "applications.menu";
			auto const menu = fmt::join({menu_prefix(), base});
			path = fmt::dir::join({config_home(), "menus", menu});
			if (env::file::fail(path))
			{
				for (auto const dir : config_dirs())
				{
					path = fmt::dir::join({dir, menu});
					if (not env::file::fail(path))
					{
						break;
					}
					path.clear();
				}
			}
		}
		return path;
	}

	fmt::string::view run_dir()
	{
		auto u = env::get("XDG_RUNTIME_DIR");
		if (empty(u))
		{
			static auto const path = fmt::dir::join({env::var::temp(), "run", env::var::user()});
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
				s = fmt::dir::join({env::var::home(), ".local", "share"});
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
		static fmt::string::view::vector t;
		auto u = env::get("XDG_CONFIG_DIRS");
		if (empty(u))
		{
			#ifdef _WIN32
			{
				static fmt::string s;
				if (empty(s))
				{
					auto const appdata = fmt::to_string(env::get("APPDATA"));
					auto const local = fmt::to_string(env::get("LOCALAPPDATA"));
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
		t = fmt::path::split(u);
		return t;
	}

	using namespace std::literals;

	fmt::string::view desktop_dir()
	{
		return dir("Desktop"sv);
	}

	fmt::string::view documents_dir()
	{
		return dir("Documents"sv);
	}

	fmt::string::view download_dir()
	{
		return dir("Downloads"sv);
	}

	fmt::string::view music_dir()
	{
		return dir("Music"sv);
	}

	fmt::string::view pictures_dir()
	{
		return dir("Pictures"sv);
	}

	fmt::string::view public_dir()
	{
		return dir("Public"sv);
	}

	fmt::string::view templates_dir()
	{
		return dir("Templates"sv);
	}

	fmt::string::view videos_dir()
	{
		return dir("Videos"sv);
	}
}

#ifdef test_unit
#include "arg.hpp"

namespace
{
	constexpr auto eol = '\n';

	struct hdr : fmt::struct_brief<hdr>
	{
		fmt::string::view group;

		hdr(fmt::string::view g)
			: group(g)
		{ }
	};

	fmt::string::out::ref operator<<(fmt::string::out::ref out, hdr::cref obj)
	{
		return out << '[' << obj.group << ']' << eol;
	}

	struct kv : fmt::struct_brief<kv>
	{
		fwd::pair<fmt::string::view> entry;

		kv(fmt::string::view key, fmt::string::view value)
			: entry(key, value)
		{ }
	};

	fmt::string::out::ref operator<<(fmt::string::out::ref out, kv::cref obj)
	{
		return out << obj.entry.first << '=' << obj.entry.second << eol;
	}
}

test_unit(usr)
{
	std::ofstream out { ".ini" };

	out	<< hdr("Fake Environment")
		<< kv("home", env::var::home())
		<< kv("user", env::var::user())
		<< kv("root", env::var::root())
		<< kv("domain", env::var::domain())
		<< kv("host", env::var::host())
		<< kv("pwd", env::var::pwd())
		<< kv("lang", env::var::lang())
		<< kv("shell", env::var::shell())
		<< kv("tmpdir", env::var::temp())
		<< kv("rootdir", env::var::base())
		<< kv("session", env::var::session())
		<< std::endl;

	out	<< hdr("Desktop")
		<< kv("runtime-dir", env::usr::run_dir())
		<< kv("data-home", env::usr::data_home())
		<< kv("config-home", env::usr::config_home())
		<< kv("cache-home", env::usr::cache_home())
		<< kv("data-dirs", fmt::path::join(env::usr::data_dirs()))
		<< kv("config-dirs", fmt::path::join(env::usr::config_dirs()))
		<< std::endl;

	out	<< hdr("User Directories")
		<< kv("desktop-dir", env::usr::desktop_dir())
		<< kv("documents-dir", env::usr::documents_dir())
		<< kv("download-dir", env::usr::download_dir())
		<< kv("music-dir", env::usr::music_dir())
		<< kv("pictures-dir", env::usr::pictures_dir())
		<< kv("publicshare-dir", env::usr::public_dir())
		<< kv("template-dir", env::usr::templates_dir())
		<< kv("videos-dir", env::usr::videos_dir())
		<< std::endl;

	out	<< hdr("Application Options")
		<< kv("name", env::opt::application())
		<< kv("program", env::opt::program())
		<< kv("command-line", fmt::join(env::opt::arguments(), " "))
		<< kv("config", env::opt::config())
		<< std::endl;

	out << env::opt::put << std::endl;
}
#endif