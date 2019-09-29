#ifndef cli_hpp
#define cli_hpp

#include "ipc.hpp"
#include "dir.hpp"
#include "tmp.hpp"

namespace env::cli
{
	using file::process;
	
	process where(fmt::view program);
	process find(fmt::view pattern);
}

#endif // file
