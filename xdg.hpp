#ifndef xdg_hpp
#define xdg_hpp

#include "env.hpp"

namespace xdg
{
	extern env::view const& data_home;
	extern env::view const& config_home;
	extern env::list const& data_dirs;
	extern env::list const& config_dirs;
	extern env::view const& cache_home;
}

#endif // file

