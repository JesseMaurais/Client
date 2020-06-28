#ifndef usr_hpp
#define usr_hpp "User Environment"

#include "fmt.hpp"

namespace env::usr
{
	fmt::string::view current_desktop();
	fmt::string::view menu_prefix();
	fmt::string::view applications_menu();
	fmt::string::view run_dir();
	fmt::string::view data_home();
	fmt::string::view config_home();
	fmt::string::view cache_home();
	fmt::string::view::span data_dirs();
	fmt::string::view::span config_dirs();
	fmt::string::view desktop_dir();
	fmt::string::view documents_dir();
	fmt::string::view download_dir();
	fmt::string::view music_dir();
	fmt::string::view pictures_dir();
	fmt::string::view publicshare_dir();
	fmt::string::view templates_dir();
	fmt::string::view videos_dir();
}

#endif // file
