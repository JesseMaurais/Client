#ifndef usr_hpp
#define usr_hpp "User Environment"

#include "fmt.hpp"

namespace env::usr
{
	fmt::string::view dir(fmt::string::view);
	fmt::string::view::span dirs(fmt::string::view);
	fmt::string::view current_desktop();
	fmt::string::view menu_prefix();
	fmt::string::view applications_menu();
	fmt::string::view run_dir();
	fmt::string::view data_home();
	fmt::string::view config_home();
	fmt::string::view cache_home();
	fmt::string::view::span data_dirs();
	fmt::string::view::span config_dirs();

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
