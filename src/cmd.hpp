#ifndef cmd_hpp
#define cmd_hpp

#include "str.hpp"
#include "process.hpp"

namespace env::cmd
{
	fmt::string::vector which(fmt::string::view name);
	/// Paths to executables with program $name

	struct open : env::file::process
	/// Preferred application at $path
	{
		explicit open(fmt::string::view path);

	private:

		using process::process;
	};
}

#endif // file
