#ifndef desktop_hpp
#define desktop_hpp

#include "str.hpp"
#include "process.hpp"

namespace env::desktop
{
	using env::file::process;
	process open(fmt::string_view path);
	fmt::string which(fmt::string_view name);
}

#endif // file
