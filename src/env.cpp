// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "env.hpp"
#include "usr.hpp"
#include "opt.hpp"
#include "arg.hpp"
#include "ini.hpp"
#include "dir.hpp"
#include "fmt.hpp"
#include "str.hpp"
#include "type.hpp"
#include "sys.hpp"
#include "err.hpp"
#include "sync.hpp"
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

namespace
{
	sys::rwlock lock;

	constexpr auto Desktop = "Desktop";
	constexpr auto Documents = "Documents";
	constexpr auto Downloads = "Downloads";
	constexpr auto Music = "Music";
	constexpr auto Pictures = "Pictures";
	constexpr auto PublicShare = "Public";
	constexpr auto Templates = "Templates";
	constexpr auto Videos = "Videos";

	auto cached(fmt::string::view var, fmt::string::view val)
	{
		auto const w = fmt::put(var);
		auto u = env::opt::get(w);
		if (u.empty())
		{
			constexpr auto base = "user-dirs.dirs";
			auto const path = fmt::dir::join({env::usr::config_home(), base});
			std::ifstream in(path);
			fmt::string line;
			while (doc::ini::getline(in, line))
			{
				constexpr char quote = '"';
				auto const first = line.find_first_not_of(quote);
				auto const second = line.find_first_of(quote, first);
				line = line.substr(first, second);
				auto const entry = fmt::to_pair(line);
				auto const value = env::var::value(entry.second);
				auto const key = fmt::set(entry.first);
				env::opt::set(key, value);
			}
			u = env::opt::get(w, val);
		}
		return u;
	}

	auto folder(fmt::string::view var, fmt::string::view val)
	{
		auto u = env::var::get(var);
		if (empty(u))
		{
			u = cached(var, val);
		}

		#ifdef _WIN32
		{
			if (empty(u))
			{
				static std::map<fmt::string::view, KNOWNFOLDERID> map =
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
			s = fmt::dir::join({env::home(), val});
			u = s;
		}
		return u;
	}
}

namespace env::var
{
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

	static auto evaluate(fmt::string::view u)
	{
		assert(u.front() == '$');
		u = u.substr(1);
		return empty(u) ? u : env::var::get(u);
	}

	fmt::string value(fmt::string::view u)
	{
		static std::regex x { "\\$[A-Z_][A-Z_0-9]*" };
		std::smatch m;
		std::string r;
		auto s = fmt::to_string(u);
		while (std::regex_search(s, m, x))
		{
			r += m.prefix();
			auto t = m.str();
			auto v = evaluate(t);
			r.append(v.data(), v.size());
			s = m.suffix().str();
		}
		r.shrink_to_fit();
		return r;
	}
}

namespace env
{
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
			if (sys::uni::fail(gethostname(name, sizeof name)))
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
			if (sys::uni::fail(getdomainname(name, sizeof name)))
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
	std::ofstream out { "Tests.ini" };

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
		<< env::opt::put
		<< std::endl;
}

test_unit(env)
{
	assert(env::var::get("PATH") == fmt::path::join(env::paths()));
	assert(env::var::get("PATH") == env::var::value("$PATH"));
}

#endif
