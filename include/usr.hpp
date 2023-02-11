#ifndef usr_hpp
#define usr_hpp "User Environment"

#include "fmt.hpp"

namespace env::usr
{
	fmt::view dir(fmt::view);
	// Directory by special name

	fmt::view path(fmt::view);
	// Path under user and job group

	fmt::view current_desktop();
	fmt::view menu_prefix();
	fmt::view applications_menu();
	fmt::view run_dir();
	fmt::view data_home();
	fmt::view config_home();
	fmt::view cache_home();
	fmt::span data_dirs();
	fmt::span config_dirs();

	inline auto desktop_dir()
	{
		return dir("Desktop");
	}

	inline auto documents_dir()
	{
		return dir("Documents");
	}

	inline auto download_dir()
	{
		return dir("Downloads");
	}

	inline auto music_dir()
	{
		return dir("Music");
	}

	inline auto pictures_dir()
	{
		return dir("Pictures");
	}

	inline auto public_dir()
	{
		return dir("Public");
	}

	inline auto templates_dir()
	{
		return dir("Templates");
	}

	inline auto videos_dir()
	{
		return dir("Videos");
	}
}

#endif // file
