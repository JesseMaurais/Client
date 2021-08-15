// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "env.hpp"
#include "usr.hpp"
#include "opt.hpp"
#include "arg.hpp"
#include "ini.hpp"
#include "cmd.hpp"
#include "dir.hpp"
#include "ps.hpp"
#include "fmt.hpp"
#include "type.hpp"
#include "str.hpp"
#include "sys.hpp"
#include "err.hpp"
#include "sync.hpp"
#include <exception>
#include <fstream>
#include <vector>
#include <regex>

#ifdef _WIN32
#include <shlobj.h>
#ifdef _MSC_VER
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")
#endif // _MSC_VER
#endif // _WIN32

namespace env::os
{
	fmt::string::view dir(fmt::string::view path, bool traced)
	{
		#ifdef trace
		if (traced) trace(path);
		#endif

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

		#endif // OS

		if (empty(u))
		{
			if (auto v = it->second.second; not empty(v))
			{
				u = env::echo(v);
			}
		}
		return u;
	}

	fmt::string::view::span dirs(fmt::string::view path, bool traced)
	{
		auto u = dir(path, traced);
		thread_local fmt::string::view::vector w;
		w = fmt::dir::split(u);
		return w;
	}
}

namespace env
{
	static sys::rwlock lock;

	bool got(fmt::string::view u)
	{
		if (not fmt::terminated(u))
		{
			auto const s = fmt::to_string(u);
			return env::got(s);
		}
		auto const c = u.data();
		auto const unlock = lock.read();
		auto const ptr = std::getenv(c);
		return nullptr == ptr;
	}

	fmt::string::view get(fmt::string::view u)
	{
		if (not fmt::terminated(u))
		{
			auto const s = fmt::to_string(u);
			return env::get(s);
		}
		auto const c = u.data();
		auto const unlock = lock.read();
		auto const ptr = std::getenv(c);
		return nullptr == ptr ? "" : ptr;
	}

	bool set(fmt::string::view u)
	{
		if (not fmt::terminated(u))
		{
			return env::put(u);
		}
		auto const unlock = lock.write();
		auto const d = u.data();
		auto c = const_cast<char*>(d);
		return 0 != sys::putenv(c);
	}

	bool put(fmt::string::view u)
	{
		static fmt::string::set buf;
		auto const unlock = lock.write();
		auto it = buf.emplace(u).first;
		auto d = it->data();
		auto c = const_cast<char*>(d);
		return 0 != sys::putenv(c);
	}

	bool put(fmt::string::view u, fmt::string::view v)
	{
		return env::put(fmt::join({u, v}, "="));
	}

	fmt::string::view echo(fmt::string::view u)
	{
		auto const p = env::shell().echo(u);
		return p.empty() ? fmt::empty : p[0];
	}
}

namespace env::var
{
	fmt::string::view::span all()
	{
		static thread_local fmt::string::view::vector local;
		local.clear();
		for (char** c = ::sys::environment(); *c; ++c) local.emplace_back(*c);
		return local;
	}

	fmt::string::view::span path()
	{
		static thread_local fmt::string::view::vector t;
		auto u = env::get("PATH");
		t = fmt::path::split(u);
		return t;
	}

	fmt::string::view temp()
	{
		for (auto u : { "TMPDIR", "TEMP", "TMP" })
		{
			if (auto v = env::get(u); not empty(v))
			{
				return v;
			}
		}
		static fmt::string s;
		if (empty(s))
		{
			#ifdef _WIN32
			{
				s = fmt::dir::join({env::var::root(), "Temp"});
			}
			#else
			{
				s = fmt::dir::join({env::var::base(), "tmp"});
			}
			#endif
		}
		return s;
	}

	fmt::string::view pwd()
	{
		#ifdef _WIN32
		{
			static thread_local char buf[FILENAME_MAX];
			return sys::getcwd(buf, sizeof buf);
		}
		#else
		{
			return env::get("PWD");
		}
		#endif
	}

	fmt::string::view home()
	{
		#ifdef _WIN32
		{
			return env::get("USERPROFILE");
		}
		#else
		{
			return env::get("HOME");
		}
		#endif
	}

	fmt::string::view user()
	{
		#ifdef _WIN32
		{
			return env::get("USERHOME");
		}
		#else
		{
			return env::get("USER");
		}
		#endif
	}

	fmt::string::view host()
	{
		#ifdef _WIN32
		{
			return env::get("COMPUTERNAME");
		}
		#else
		{
			static thread_local char buf[64];
			if (sys::fail(gethostname(buf, sizeof buf)))
			{
				sys::err(here, "gethostname");
				std::memset(buf, 0, sizeof buf);
			}
			return buf;
		}
		#endif
	}

	fmt::string::view domain()
	{
		#ifdef _WIN32
		{
			return env::get("USERDOMAIN");
		}
		#else
		{
			static thread_local char buf[64];
			if (sys::fail(getdomainname(buf, sizeof buf)))
			{
				sys::err(here, "getdomainname");
				std::memset(buf, 0, sizeof buf);
			}
			return buf;
		}
		#endif
	}

	fmt::string::view base()
	{
		#ifdef _WIN32
		{
			return env::get("SYSTEMDRIVE");
		}
		#else
		{
			return fmt::empty; // omit "/" for join
		}
		#endif
	}

	fmt::string::view root()
	{
		#ifdef _WIN32
		{
			return env::get("SYSTEMROOT");
		}
		#else
		{
			return "/root";
		}
		#endif
	}

	fmt::string::view lang()
	{
		for (auto u : { "LC_ALL", "LC_MESSAGES", "LANG" })
		{
			if (auto v = env::get(u); not empty(v))
			{
				return v;
			}
		}
		return fmt::empty;
	}

	fmt::string::view shell()
	{
		#ifdef _WIN32
		{
			return env::get("ComSpec");
		}
		#else
		{
			return env::get("SHELL");
		}
		#endif
	}

	fmt::string::view session()
	{
		#ifdef _WIN32
		{
			return env::get("OS");
		}
		#else
		{
			return env::get("DESKTOP_SESSION");
		}
		#endif
	}
}

#ifdef test_unit
test_unit(env)
{
	assert(env::get("PATH") == fmt::path::join(env::var::path()));
	assert(env::get("PATH") == env::echo("PATH"));
}
#endif
