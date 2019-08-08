#ifndef opt_hpp
#define opt_hpp

#include "env.hpp"

namespace env::opt
{
	extern fmt::string application_name;
	extern env::view const& configuration;
	extern env::view const& application;
	extern env::list const& arguments;
	extern env::view const& program;
	void init(int argc, char** argv);

	fmt::string_view key(fmt::string_view);
};

#endif // file
