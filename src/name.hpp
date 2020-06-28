#ifndef name_hpp
#define name_hpp

namespace env
{
	enum class name
	{
		// env.hpp
		paths, 
		home, 
		pwd, 
		tmpdir, 
		user, 
		host, 
		domain, 
		lang, 
		shell, 
		session,
		// usr.hpp
		current_desktop,
		menu_prefix,
		applications_menu,
		run_dir,
		data_home,
		config_home,
		cache_home,
		data_dirs,
		config_dirs,
		desktop_dir,
		documents_dir,
		download_dir,
		music_dir,
		pictures_dir,
		publicshare_dir,
		templates_dir,
		videos_dir,
		// arg.hpp
		application,
		arguments,
		initials,
		program,
		config,
		cache,
		data,
		rundir,
		// x11.hpp
		authority,
	};
};

#endif // file
