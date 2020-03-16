#ifndef cmd_hpp
#define cmd_hpp

#include "str.hpp"
#include "process.hpp"

namespace env::cmd
{
	fmt::vector_string which(fmt::string_view name);
	/// Paths to executables with program $name

	struct open : env::file::process
	/// Preferred application at $path
	{
		explicit open(fmt::string_view path);

	private:

		using process::process;
	};
}

#endif // file
