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
#include <shlobj.h>
#endif

namespace
{
	struct : env::view
	{
		operator type() const final
		{
			auto u = env::var::get("XDG_CURRENT_DESKTOP");
			if (empty(u))
			{
				u = env::session;
			}
			return u;
		}

	} CURRENT_DESKTOP;

	struct : env::view
	{
		operator type() const final
		{
			auto u = env::var::get("XDG_MENU_PREFIX");
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
		operator type() const final
		{
			static fmt::string path;
			if (empty(path))
			{
				constexpr auto base = "applications.menu";
				auto const menu = fmt::join({env::usr::menu_prefix, base});
				path = fmt::dir::join({env::usr::config_home, "menus", menu});
				if (env::file::fail(path))
				{
					fmt::span<fmt::string_view> const dirs = env::usr::config_dirs;
					for (auto const dir : dirs)
					{
						path = fmt::dir::join({dir, menu});
						if (not env::dir::fail(path))
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
		operator type() const final
		{
			auto u = env::var::get("XDG_RUNTIME_DIR");
			if (empty(u))
			{
				static auto const path = fmt::dir::join({env::tmpdir, "run", env::user});
				u = path;
			}
			return u;
		}

	} RUN_DIR;

	struct : env::view
	{
		operator type() const final
		{
			auto u = env::var::get("XDG_DATA_HOME");
			if (empty(u))
			{
				static fmt::string s;
				if (empty(s))
				{
					s = fmt::dir::join({env::home, ".local", "share"});
				}
				u = s;
			}
			return u;
		}

	} DATA_HOME;

	struct : env::view
	{
		operator type() const final
		{
			auto u = env::var::get("XDG_CONFIG_HOME");
			if (empty(u))
			{
				static fmt::string s;
				if (empty(s))
				{
					s = fmt::dir::join({env::home, ".config"});
				}
				u = s;
			}
			return u;
		}

	} CONFIG_HOME;

	struct : env::view
	{
		operator type() const final
		{
			auto u = env::var::get("XDG_CACHE_HOME");
			if (empty(u))
			{
				static fmt::string s;
				if (empty(s))
				{
					s = fmt::dir::join({env::home, ".cache"});
				}
				u = s;
			}
			return u;
		}

	} CACHE_HOME;

	struct : env::span
	{
		operator type() const final
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

	} DATA_DIRS;

	struct : env::span
	{
		operator type() const final
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
		operator type() const final
		{
			auto u = env::var::get(var);
			if (empty(u))
			{
				u = cached();
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
				s = fmt::dir::join({env::home, val});
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

		fmt::string::view cached() const
		{
			auto u = env::opt::get(var);
			if (empty(u))
			{
				constexpr auto base = "user-dirs.dirs";
				auto const path = fmt::dir::join({env::usr::config_home, base});
				std::ifstream in(path);
				fmt::string line;
				while (doc::ini::getline(in, line))
				{
					constexpr char quote = '"';
					auto const first = line.find_first_not_of(quote);
					auto const second = line.find_first_of(quote, first);
					line = line.substr(first, second);
					auto entry = fmt::entry(line);
					auto value = env::var::value(entry.second);
					env::opt::set(entry.first, value);
				}
				u = env::opt::get(var);
			}
			return u;
		}
	};

	user_dir DESKTOP_DIR("XDG_DESKTOP_DIR", Desktop);
	user_dir DOCUMENTS_DIR("XDG_DOCUMENTS_DIR", Documents);
	user_dir DOWNLOAD_DIR("XDG_DOWNLOAD_DIR", Downloads);
	user_dir MUSIC_DIR("XDG_MUSIC_DIR", Music);
	user_dir PICTURES_DIR("XDG_PICTURES_DIR", Pictures);
	user_dir PUBLICSHARE_DIR("XDG_PUBLICSHARE_DIR", PublicShare);
	user_dir TEMPLATES_DIR("XDG_TEMPLATES_DIR", Templates);
	user_dir VIDEOS_DIR("XDG_VIDEOS_DIR", Videos);
}

namespace env::usr
{
	view::ref current_desktop = CURRENT_DESKTOP;
	view::ref menu_prefix = MENU_PREFIX;
	view::ref applications_menu = APPLICATIONS_MENU;
	view::ref run_dir = RUN_DIR;
	view::ref data_home = DATA_HOME;
	view::ref config_home = CONFIG_HOME;
	view::ref cache_home = CACHE_HOME;
	span::ref data_dirs = DATA_DIRS;
	span::ref config_dirs = CONFIG_DIRS;
	view::ref desktop_dir = DESKTOP_DIR;
	view::ref documents_dir = DOCUMENTS_DIR;
	view::ref download_dir = DOWNLOAD_DIR;
	view::ref music_dir = MUSIC_DIR;
	view::ref pictures_dir = PICTURES_DIR;
	view::ref publicshare_dir = PUBLICSHARE_DIR;
	view::ref templates_dir = TEMPLATES_DIR;
	view::ref videos_dir = VIDEOS_DIR;
}

#ifndef NDEBUG
namespace
{
	constexpr auto eol = '\n';

	struct hdr
	{
		fmt::string::view group

		hdr(fmt::string::view g)
			: group(g)
		{ }

		fmt::string::out operator()(fmt::string::out out)
		{
			return out << '[' << group << ']' << eol;
		}
	};

	struct kv : fwd::pair<fmt::string::view>
	{
		using pair::pair

		fmt::string::out operator()(fmt::string::out out)
		{
			return out << first << '=' << second << eol;
		}
	};
}

void test::run<test::unit::usr>() noexcept
{
	std::ofstream out { "Tests.ini" };

	out	<< hdr("Fake Environment")
		<< kv("home", env::home)
		<< kv("user", env::user)
		<< kv("root", env::root)
		<< kv("domain", env::domain)
		<< kv("host", env::host)
		<< kv("pwd", env::pwd)
		<< kv("lang", env::lang)
		<< kv("shell", env::shell)
		<< kv("tmpdir", env::tmpdir)
		<< kv("rootdir", env::rootdir)
		<< kv("session", env::session)
		<< kv("prompt", env::prompt)
		<< std::endl;

	out	<< hdr("Desktop")
		<< kv("runtime-dir", env::usr::rundir)
		<< kv("data-home", env::usr::data_home)
		<< kv("config-home", env::usr::config_home)
		<< kv("cache-home", env::usr::cache_home)
		<< kv("data-dirs", fmt::path::join(env::usr::data_dirs))
		<< kv("config-dirs", fmt::path::join(env::usr::config_dirs))
		<< std::endl;

	out	<< hdr("User Directories")
		<< kv("desktop-dir", env::usr::desktop_dir)
		<< kv("documents-dir", env::usr::documents_dir)
		<< kv("download-dir", env::usr::download_dir)
		<< kv("music-dir", env::usr::music_dir)
		<< kv("pictures-dir", env::usr::pictures_dir)
		<< kv("publicshare-dir", env::usr::publicshare_dir)
		<< kv("template-dir", env::usr::templates_dir)
		<< kv("videos-dir", env::usr::videos_dir)
		<< std::endl;

	out	<< hdr("Application Options")
		<< kv("name", env::opt::application)
		<< kv("program", env::opt::program)
		<< kv("command-line", fmt::join(env::opt::arguments, " "));
		<< kv("config", env::opt::config)
		<< env::opt::put
		<< std::endl;
}
#endif
