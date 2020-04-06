#ifndef shell_hpp
#define shell_hpp "Environment Shell"

#include "str.hpp"
#include "env.hpp"
#include "process.hpp"

namespace env::shell
{
	using fmt::string;
	using string::view;
	using string::span;
	using string::ref;
	using string::in;
	using string::out;

	struct open : env::file::process
	// With desktop program for $path
	{
		explicit open(view path);

	private:

		using process::process;
	};

	span get(in::ref);
	// Lines read $in

	span dir(view path);
	// Contents of directory given by $path

	span which(view name);
	// Paths to executables with program $name
}

#endif // file
