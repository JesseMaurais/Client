#ifndef usr_hpp
#define usr_hpp

#include "env.hpp"

namespace env::usr
{
	extern view::ref current_desktop;
	extern view::ref menu_prefix;
	extern view::ref applications_menu;
	extern view::ref run_dir;
	extern view::ref data_home;
	extern view::ref config_home;
	extern view::ref cache_home;
	extern span::ref data_dirs;
	extern span::ref config_dirs;
	extern view::ref desktop_dir;
	extern view::ref documents_dir;
	extern view::ref download_dir;
	extern view::ref music_dir;
	extern view::ref pictures_dir;
	extern view::ref publicshare_dir;
	extern view::ref templates_dir;
	extern view::ref videos_dir;
}

#endif // file
