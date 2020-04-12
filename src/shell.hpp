#ifndef shell_hpp
#define shell_hpp "Environment Shell"

#include "env.hpp"
#include "process.hpp"

namespace env
{
	using fmt::string;
	using string::view;
	using string::span;
	using string::ref;
	using string::in;
	using string::out;

	struct shell : file::process
	// With desktop program for $path
	{
		explicit shell(view line);
		// Start command line

		static span get(in::ref);
		// Lines read $in

		static span dir(view path);
		// Contents of directory given by $path

		static span which(view name);
		// Paths to executables with program $name

	private:

		using process::process;
	};
}

#endif // file
