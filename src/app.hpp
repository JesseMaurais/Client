#ifndef app_hpp
#define app_hpp

#include "str.hpp"
#include "process.hpp"

namespace env::app
{
	using env::file::process;
	process open(fmt::string_view path);
	fmt::string which(fmt::string_view name);
}

#endif // file
