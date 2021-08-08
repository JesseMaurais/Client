#ifndef env_hpp
#define env_hpp "Environment Variables"
 
#include "fmt.hpp"

namespace env::os
{
	fmt::string::view::span dirs(fmt::string::view direct, bool traced = false);
	/// Evaluate $direct as interpreted by operating system
	/// Write recusive error diagnostic if $traced
	/// WIN32: environment is %embraced%
	/// POSIX: environment is $prefaced

	fmt::string::view dir(fmt::string::view direct, bool traced = false);
	/// The first match in paths
}

namespace env
{
	namespace var
	{
		bool got(fmt::string::view);
		fmt::string::view get(fmt::string::view);
		bool set(fmt::string::view);
		bool put(fmt::string::view);
		bool put(fmt::string::view, fmt::string::view);
		fmt::string::view value(fmt::string::view);
	}

	fmt::string::view::span vars();
	fmt::string::view::span paths();
	fmt::string::view temp();
	fmt::string::view pwd();
	fmt::string::view base();
	fmt::string::view root();
	fmt::string::view home();
	fmt::string::view user();
	fmt::string::view host();
	fmt::string::view lang();
	fmt::string::view shell();
	fmt::string::view domain();
	fmt::string::view session();
}

#endif // file
