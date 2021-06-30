// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "env.hpp"
#include "usr.hpp"
#include "opt.hpp"
#include "arg.hpp"
#include "ini.hpp"
#include "cmd.hpp"
#include "dev.hpp"
#include "dlg.hpp"
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
	fmt::string::view::span dir(fmt::string::view path, bool traced)
	{
		#ifdef trace
		if (traced) trace(path);
		#endif

		fmt::string::view u = fmt::empty;
		
		#ifdef _WIN32
		static std::map
		<
			fmt::string::view, std::pair<KNOWNFOLDERID, fmt::string::view::vector>
		>
		const map =
		{
			{ "AccountPictures", {FOLDERID_AccountPictures, "%AppData%\\Microsoft\\Winodws\\AccountPictures"}},
			{ "AdminTools", {FOLDERID_AdminTools, "%AppData%\\Microsoft\\Windows\\Start Menu\\Programs\\Administrative Tools"}},
			{ "AppDataDesktop", {FOLDERID_AppDataDesktop, "%LocalAppData%\\Desktop"}},
			{ "AppDataDocuments", {FOLDERID_AppDataDocuments, "%LocalAppData%\\Documents"}},
			{ "AppDataFavorites", {FOLDERID_AppDataDocuments, "%LocalAppData%\\Favorites"}},
			{ "AppDataProgramData", {FOLDERID_AppDataProgramData, "%LocalAppData%\\ProgramData"}},
			{ "ApplicationShortcuts", {FOLDERID_ApplicationShortcuts, "%LocalAppData%\\Microsoft\\Windows\\Application Shortcuts"}},
			{ "CDBurning", {FOLDERID_CDBurning, "%LocalAppData%\\Microsoft\\Windows\\Burn\\Burn"}},
			{ "CommonAdminTools", {FOLDERID_CommonAdminTools, "%AllUsersProfile%\\Start Menu\\Programs\\Administrative Tools"}},
			{ "CommonPrograms", {FOLDERID_CommonStartMenu, "%AllUsersProfile%\\Start Menu\\Programs"}},
			{ "CommonStartMenu", {FOLDERID_CommonStartMenu, "%AllUsersProfile%\\Start Menu"}},
			{ "CommonStartup", {FOLDERID_CommonStartup, "%AllUsersProfile%\\Start Menu\\StartUp"}},
			{ "CommonTemplates" {FOLDERID_CommonTemplates, "%AllUsersProfile%\\Templates"}},
			{ "LocalAppData", {FOLDERID_LocalAppData, "%UserProfile%\\AppData\\Local"}},
			{ "Camera", {FOLDERID_CameraRoll, "%UserProfile%\\Pictures\\Camera Roll"}},
			{ "Contacts", {FOLDERID_Contacts, "%UserProfile%\\Contacts"}},
			{ "Cookies", {FOLDERID_Cookies, "%UserProfile%\\Cookies"}},
			{ "Desktop", {FOLDERID_Desktop, "%UserProfile%\\Desktop"}},
			{ "Documents", {FOLDERID_Documents, "%UserProfile%\\Documents"}},
			{ "Downloads", {FOLDERID_Downloads, "%UserProfile%\\Downloads"}},
			{ "Favorites", {FOLDERID_Favorites, "%UserProfile%\\Favorites"}},
			{ "Fonts", {FOLDERID_Fonts, "%WinDir%\\Fonts" }},
			{ "History", {FOLDERID_History, "%UserProfile%\\Local Settings\\History"}},
			{ "InternetCache", {FOLDERID_InternetCache, "%LocalAppData%\\Local Settings\\Temporary"}},
			{ "Links", {FOLDERID_Links}, "%UserProfile%\\Links"}},
			{ "Objects3D", {FOLDERID_Objects3D, "%UserProfile%\\Objects3D"}},
			{ "Resources", {FOLDERID_LocalizedResourcesDir, "%WinDir%\\Resources"}},
			{ "Music", {FOLDERID_Music, "%UserProfile%\\Music;%UserProfile%\\My Documents\\My Music"}},
			{ "Pictures", {FOLDERID_Pictures, "%UserProfile%\\Pictures;%UserProfile%\\My Documents\\My Pictures"}},
			{ "Profile", {FOLDERID_Profile, "%UserProfile%"}},
			{ "ProgramData", {FOLDERID_ProgramData, "%AllUsersProfile%\\Application Data;%AllUsersProfile%;%ProgramData%;%SystemDrive%\\ProgramData"}}},
			{ "ProgramFilesCommon", {FOLDERID_ProgramFilesCommon, "%ProgramFiles%\\Common Files"}},
			{ "ProgramFiles", {FOLDERID_ProgramFiles, "%ProgramFiles%;%SystemDrive%\\Program Files"}},
			{ "Programs", {FOLDERID_Programs, "%UserProfile%\\Start Menu\\Programs"}},
			{ "Public", {FOLDERID_Public, "%Public%", "%SystemDrive%\\Users\\Public"}},
			{ "PublicDesktop", {FOLDERID_PublicDesktop, "%Public%\\Desktop;%AllUsersProfile%\\Desktop"}},
			{ "PublicDocuments", {FOLDERID_PublicDocuments, "%Public%\\Documents;%AllUsersProfile%\\Documents"}},
			{ "PublicDownloads", {FOLDERID_PublicDownloads, "%Public%\\Downloads"}},
			{ "Screenshot", {FOLDERID_Screenshots, "%UserProfile%\\Pictures\\Screenshots"}},
			{ "StartMenu", {FOLDERID_StartMenu, "%UserProfile%\\Start Menu"}},
			{ "Startup", {FOLDERID_Startup, "%UserProfile%\\Start Menu\\Programs\\StartUp"}},
			{ "System", {FOLDERID_System, "%WinDir%\\System32"}},
			{ "Templates", {FOLDERID_Templates, "%UserProfile%\\Templates"}},
			{ "UserProfiles", {FOLDERID_UserProfiles, "%SystemDrive%\\Users"}},
			{ "UserProgramFiles", {FOLDERID_UserProgramFiles, "%LocalAppData%\\Programs"}}
			{ "UserProgramFilesCommon", {FOLDERID_UserProgramFilesCommon, "%LocalAppData%\\Programs\\Common"}},
			{ "Videos", {FOLDERID_Videos, "%UserProfile%\\Videos;%UserProfile%\\My Documents\\My Videos"}},
			{ "Windows", {FOLDERID_Windows, "%WinDir%"}}
		};

		auto it = map.find(path);
		if (map.end() != it)
		{
			PWSTR pws = nullptr;
			auto const ok = SHGetKnownFolderPath
			(
				it->first, KF_FLAG_DEFAULT, nullptr, &pws
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
			fmt::string::view, std::pair<fmt::string::view, fmt::string::view>
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
			u = env::var::get(it->first);
		}

		#endif // OS

		if (empty(u))
		{
			for (auto v : it->second)
			{
				u = env::var::value(v);
				if (not empty(u)) 
				{
					break;
				}
			}
		}

		thread_local fmt::string::view::vector w;
		w = fmt::dir::split(u);
		return w;
	}
}

namespace env::var
{
	static sys::rwlock lock;

	bool got(fmt::string::view u)
	{
		if (not fmt::terminated(u))
		{
			auto const s = fmt::to_string(u);
			return env::var::got(s);
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
			return env::var::get(s);
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
			return env::var::put(u);
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
		return env::var::put(fmt::join({u, v}, "="));
	}

	fmt::string::view value(fmt::string::view u)
	{
		thread_local fmt::string s;
		fmt::string::stream ss;

		#ifdef _WIN32
		{
			constexpr auto x = "%";

			for (auto [i, j] = fmt::embrace(u, x); i < j; std::tie(i, j) = fmt::embrace(u, x))
			{
				auto const prefix = u.substr(0, i);
				auto const infix = u.substr(i + 1, j);
				auto const suffix = u.substr(j + 1);
				auto const value = get(infix);
				ss << prefix << value << suffix;
			}
		}
		#else // POSIX
		{
			static std::regex x { "\\$[A-Z_][A-Z_0-9]*" };
			std::smatch m;
			
			auto s = fmt::to_string(u);
			while (std::regex_search(s, m, x))
			{
				auto const prefix = m.prefix();
				auto const infix = m.str().substr(1);
				auto const suffix = m.suffix();
				auto const value = get(infix);
				ss << prefix << value << suffix;
			}
		}
		#endif

		s = ss.str();
		return s;
	}
}

namespace env
{
	fmt::string::view::span vars()
	{
		static thread_local fmt::string::view::vector local;
		local.clear();
		for (auto c = sys::environ(); *c; ++c) local.emplace_back(c);
		return local;
	}

	fmt::string::view::span paths()
	{
		static thread_local fmt::string::view::vector t;
		auto u = env::var::get("PATH");
		t = fmt::path::split(u);
		return t;
	}

	fmt::string::view temp()
	{
		for (auto u : { "TMPDIR", "TEMP", "TMP" })
		{
			auto v = env::var::get(u);
			if (not empty(v))
			{
				return v;
			}
		}
		static fmt::string s;
		if (empty(s))
		{
			#ifdef _WIN32
			{
				s = fmt::dir::join({env::root(), "Temp"});
			}
			#else
			{
				s = fmt::dir::join({env::base(), "tmp"});
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
			return env::var::get("PWD");
		}
		#endif
	}

	fmt::string::view home()
	{
		#ifdef _WIN32
		{
			return env::var::get("USERPROFILE");
		}
		#else
		{
			return env::var::get("HOME");
		}
		#endif
	}

	fmt::string::view user()
	{
		#ifdef _WIN32
		{
			return env::var::get("USERNAME");
		}
		#else
		{
			return env::var::get("USER");
		}
		#endif
	}

	fmt::string::view host()
	{
		#ifdef _WIN32
		{
			return env::var::get("COMPUTERNAME");
		}
		#else
		{
			static thread_local char name[64];
			if (sys::fail(gethostname(name, sizeof name)))
			{
				sys::err(here, "gethostname");
				name[0] = fmt::null;
			}
			return name;
		}
		#endif
	}

	fmt::string::view domain()
	{
		#ifdef _WIN32
		{
			return env::var::get("USERDOMAIN");
		}
		#else
		{
			static thread_local char name[64];
			if (sys::fail(getdomainname(name, sizeof name)))
			{
				sys::err(here, "getdomainname");
				name[0] = fmt::null;
			}
			return name;
		}
		#endif
	}

	fmt::string::view base()
	{
		#ifdef _WIN32
		{
			return env::var::get("SYSTEMDRIVE");
		}
		#else
		{
			return ""; // omit "/" for join
		}
		#endif
	}

	fmt::string::view root()
	{
		#ifdef _WIN32
		{
			return env::var::get("SYSTEMROOT");
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
			auto v = env::var::get(u);
			if (not empty(v))
			{
				return v;
			}
		}
		return "";
	}

	fmt::string::view shell()
	{
		#ifdef _WIN32
		{
			return env::var::get("COMSPEC");
		}
		#else
		{
			return env::var::get("SHELL");
		}
		#endif
	}

	fmt::string::view session()
	{
		#ifdef _WIN32
		{
			return env::var::get("OS");
		}
		#else
		{
			return env::var::get("DESKTOP_SESSION");
		}
		#endif
	}
}

namespace env::usr
{
	fmt::string::view current_desktop()
	{
		auto u = env::var::get("XDG_CURRENT_DESKTOP");
		if (empty(u))
		{
			u = env::session();
		}
		return u;
	}

	fmt::string::view menu_prefix()
	{
		auto u = env::var::get("XDG_MENU_PREFIX");
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
		auto u = env::var::get("XDG_RUNTIME_DIR");
		if (empty(u))
		{
			static auto const path = fmt::dir::join({env::temp(), "run", env::user()});
			u = path;
		}
		return u;
	}

	fmt::string::view data_home()
	{
		auto u = env::var::get("XDG_DATA_HOME");
		if (empty(u))
		{
			static fmt::string s;
			if (empty(s))
			{
				s = fmt::dir::join({env::home(), ".local", "share"});
			}
			u = s;
		}
		return u;
	}

	fmt::string::view config_home()
	{
		auto u = env::var::get("XDG_CONFIG_HOME");
		if (empty(u))
		{
			static fmt::string s;
			if (empty(s))
			{
				s = fmt::dir::join({env::home(), ".config"});
			}
			u = s;
		}
		return u;
	}

	fmt::string::view cache_home()
	{
		auto u = env::var::get("XDG_CACHE_HOME");
		if (empty(u))
		{
			static fmt::string s;
			if (empty(s))
			{
				s = fmt::dir::join({env::home(), ".cache"});
			}
			u = s;
		}
		return u;
	}

	fmt::string::view::span data_dirs()
	{
		static fmt::string::view::vector t;
		auto u = env::var::get("XDG_DATA_DIRS");
		if (empty(u))
		{
			#ifdef _WIN32
			{
				u = env::var::get("ALLUSERSPROFILE");
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
		auto u = env::var::get("XDG_CONFIG_DIRS");
		if (empty(u))
		{
			#ifdef _WIN32
			{
				static fmt::string s;
				if (empty(s))
				{
					auto const appdata = fmt::to_string(env::var::get("APPDATA"));
					auto const local = fmt::to_string(env::var::get("LOCALAPPDATA"));
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

	fmt::string::view desktop_dir()
	{
		return folder("XDG_DESKTOP_DIR", Desktop);
	}

	fmt::string::view documents_dir()
	{
		return folder("XDG_DOCUMENTS_DIR", Documents);
	}

	fmt::string::view download_dir()
	{
		return folder("XDG_DOWNLOAD_DIR", Downloads);
	}

	fmt::string::view music_dir()
	{
		return folder("XDG_MUSIC_DIR", Music);
	}

	fmt::string::view pictures_dir()
	{
		return folder("XDG_PICTURES_DIR", Pictures);
	}

	fmt::string::view public_dir()
	{
		return folder("XDG_PUBLICSHARE_DIR", PublicShare);
	}

	fmt::string::view templates_dir()
	{
		return folder("XDG_TEMPLATES_DIR", Templates);
	}

	fmt::string::view videos_dir()
	{
		return folder("XDG_VIDEOS_DIR", Videos);
	}
}

namespace env
{
	// cmd.hpp

	struct shell& cmd()
	{
		return dlg();
	}

	shell::page shell::get(in put, char end, int count)
	{
		// Result in span at cache end
		auto const first = cache.size();
		auto second = first;
		try // process can crash
		{
			while (--count and std::getline(put, last, end))
			{
				cache.emplace_back(std::move(last));
			}
			// One past the end
			second = cache.size();
		}
		// Put exception message into line
		catch (std::exception const& error)
		{
			last = error.what();
		}
		page result(first, second, &cache);
		return result;
	}

	shell::page shell::run(span arguments)
	{
		fmt::ipstream sub(arguments);
		auto const line = get(sub);
		status = sub.wait();
		return line;
	}

	shell::page shell::run(init arguments)
	{
		view::vector s(arguments);
		return run(s);
	}

	shell::page shell::list(view name)
	{
		return run
		(
		#ifdef _WIN32
			{ "dir", "/b", name }
		#else
			{ "ls", name }
		#endif
		);
	}

	shell::page shell::copy(view path)
	{
		return run
		(
		#ifdef _WIN32
			{ "type", path }
		#else
			{ "cat", path }
		#endif
		);
	}

	shell::page shell::find(view pattern, view directory)
	{
		#ifdef _WIN32
		{
			return list(fmt::dir::join({directory, pattern}));
		}
		#else
		{
			return run({ "find", directory, "-type", "f", "-name", pattern });
		}
		#endif
	}

	shell::page shell::which(view name)
	{
		return run
		(
		#ifdef _WIN32
			{ "where", name }
		#else
			{ "which", "-a", name }
		#endif
		);
	}

	shell::page shell::open(view path)
	{
		#ifdef _WIN32
		{
			return run({ "start", "/d", path });
		}
		#else
		{
			string::view::pair const test [] =
			{
				{ "xfce", "exo-open" },
				{ "gnome", "gnome-open" },
				{ "kde", "kde-open" },
				{ "", "xdg-open" },
			};

			for (auto [session, program] : test)
			{
				if (empty(session) or desktop::current(session))
				{
					if (auto path = which(program); not path.empty())
					{
						continue;
					}

					return run({ program, path });
				}
			}
			return { 0, 0, &cache };
		}
		#endif
	}

	// dev.hpp

	struct dump& dev()
	{
		thread_local dump local;
		return local;
	}

	shell::page dump::sym(view path)
	{
		fmt::ipstream sub
		#ifdef _WIN32
			{ "dumpbin", "-nologo", "-imports", path };
		#else
			{ "objdump", "-t", path };
		#endif
		return get(sub);
	}

	shell::page dump::dyn(view path)
	{
		fmt::ipstream sub
		#ifdef _WIN32
			{ "dumpbin", "-nologo", "-exports", path };
		#else
			{ "objdump", "-T", path };
		#endif
		return get(sub);
	}

	// dlg.hpp

	desktop& dlg()
	{
		thread_local desktop local;
		return local;
	}

	bool desktop::current(fmt::string::view name)
	{
		auto const lower = fmt::to_lower(name);
		auto const current = fmt::to_lower(env::usr::current_desktop());
		return current.find(lower) != fmt::npos;
	}

	shell::page desktop::with(string::span command)
	{
		static fmt::name const group = fmt::put("Desktop Entry");
		static fmt::name const key = fmt::put("DIALOG");
		static fmt::pair const entry { group, key };
		static fmt::view const zenity = "zenity";

		// Look for the Zenity desktop utility program
		auto const program = env::opt::get(entry, zenity);
		if (auto const path = which(program); path.empty())
		{
			return path;
		}

		// Program is first command in paired
		fwd::vector<char const*> list;
		list.push_back(data(program));

		// Arguments null terminated
		constexpr auto del = " ";
		if (auto s = fmt::join(command, del); not empty(s))
		{
			for (auto u : fmt::split(s, del))
			{
				list.push_back(data(u));
			}
		}

		// Command list null terminated
		auto const argc = list.size();
		list.push_back(nullptr);
		auto const argv = list.data();

		// Run process with command
		fmt::ipstream sub
			{ argc, argv };
		return get(sub);
	}

	static auto param(fmt::string::view key, fmt::string::view value)
	{
		return fmt::join({key, value}, "=");
	}

	shell::page desktop::select(view path, mode mask)
	{
		vector command { "--file-selection" };

		if (not empty(path))
		{
			command.emplace_back(param("--filename", path));
		}
		if (mask == mode::many)
		{
			command.emplace_back("--multiple");
		}
		if (mask == mode::dir)
		{
			command.emplace_back("--directory");
		}
		if (mask == mode::save)
		{
			command.emplace_back("--save");
		}

		return with(command);
	}

	static auto message_type(desktop::msg type)
	{
		switch (type)
		{
		default:
		case desktop::msg::error:
			return "--error";
		case desktop::msg::info:
			return "--info";
		case desktop::msg::query:
			return "--question";
		case desktop::msg::warn:
			return "--warn";
		}
	};

	shell::page desktop::message(view text, msg type)
	{
		vector command { message_type(type) };

		if (not empty(text))
		{
			command.emplace_back(param("--text", text));
		}

		return with(command);
	}

	shell::page desktop::enter(view start, view label, bool hide)
	{
		vector command { param("--entry-text", start) };

		if (not empty(label))
		{
			command.emplace_back(param("--text", label));
		}
		if (hide)
		{
			command.emplace_back("--hide-text");
		}

		return with(command);
	}

	shell::page desktop::text(view path, view check, view font, txt type)
	{
		vector command { "--text-info" };

		if (type == txt::html)
		{
			command.emplace_back("--html");
			command.emplace_back(param("--url", path));
		}
		else
		{
			if (type == txt::edit)
			{
				command.emplace_back("--editable");
			}
			command.emplace_back(param("--filename", path));
		}
		if (not empty(font))
		{
			command.emplace_back(param("--font", font));
		}
		if (not empty(check))
		{
			command.emplace_back(param("--checkbox", check));
		}
		return with(command);

	}

	shell::page desktop::form(controls add, view text, view title)
	{
		vector command { "--forms" };

		if (not empty(text))
		{
			command.emplace_back(param("--text", text));
		}
		if (not empty(title))
		{
			command.emplace_back(param("--text", title));
		}

		std::string const prefix("--add-");
		for (auto const& ctl : add)
		{
			auto const key = prefix + fmt::to_string(ctl.second);
			command.emplace_back(param(key, ctl.first));
		}

		return with(command);
	}

	shell::page desktop::notify(view text, view icon)
	{
		vector command { "--notification" };

		if (not empty(text))
		{
			command.emplace_back(param("--text", text));
		}
		if (not empty(icon))
		{
			command.emplace_back(param("--icon", icon));
		}

		return with(command);
	}

	shell::page desktop::calendar(view text, view format, int day, int month, int year)
	{
		vector command { "--calendar" };

		if (not empty(text))
		{
			command.emplace_back(param("--text", text));
		}
		if (not empty(format))
		{
			command.emplace_back(param("--format", format));
		}
		if (0 < day)
		{
			command.emplace_back(param("--day", fmt::to_string(day)));
		}
		if (0 < month)
		{
			command.emplace_back(param("--month", fmt::to_string(month)));
		}
		if (0 < year)
		{
			command.emplace_back(param("--year", fmt::to_string(year)));
		}

		return with(command);
	}

	shell::page desktop::color(view start, bool palette)
	{
		vector command { "--color-selection" };

		if (not empty(start))
		{
			command.emplace_back(param("--color", start));
		}
		if (palette)
		{
			command.emplace_back("--show-palette");
		}

		return with(command);
	}
}

#ifdef test_unit

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
		<< kv("home", env::home())
		<< kv("user", env::user())
		<< kv("root", env::root())
		<< kv("domain", env::domain())
		<< kv("host", env::host())
		<< kv("pwd", env::pwd())
		<< kv("lang", env::lang())
		<< kv("shell", env::shell())
		<< kv("tmpdir", env::temp())
		<< kv("rootdir", env::base())
		<< kv("session", env::session())
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

test_unit(env)
{
	assert(env::var::get("PATH") == fmt::path::join(env::paths()));
	assert(env::var::get("PATH") == env::var::value("$PATH"));
}

test_unit(shell)
{
	struct env::shell sh;
	auto const list = sh.list(env::pwd());
	assert(not empty(list));
	auto const copy = sh.copy(__FILE__);
	assert(not empty(copy));
	// Copy range starts at 0, file numbering at 1
	assert(copy[__LINE__-1].find("Recursive find me text") != fmt::npos);
}

#endif
