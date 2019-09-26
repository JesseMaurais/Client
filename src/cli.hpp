#ifndef cli_hpp
#define cli_hpp

#include "ipc.hpp"
#include "tmp.hpp"

namespace env::cli
{
	extern bool const cmd;
	extern bool const sh;

	using process = ::sys::file::process;
	using entry = predicate<fmt::view>;
	
	process where(fmt::view program);
	process find(fmt::view pattern);
	
	void where(fmt::view, entry);
	void find(fmt::view, entry);
}

#endif // file
