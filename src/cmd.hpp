#ifndef cmd_hpp
#define cmd_hpp "Command Line Shell"

#include "fmt.hpp"

namespace env
{
	struct shell
	{
		using string = fmt::string;
		using view   = string::view;
		using vector = string::vector;
		using span   = view::span;
		using init   = view::init;
		using line   = fwd::line<vector>;
		using out    = string::out::ref;
		using in     = string::in::ref;

		fmt::string::vector cache;
		fmt::string last;
		int status;

		line get(in, char end = '\n', int count = 0);
		// Cache all lines in to end, return indices

		line run(init arguments);
		// Run command as sub process

		line run(span arguments);
		// Run command as sub process

		line list(view directory = ".");
		// List files in directory

		line copy(view path);
		// Lines in file given by path

		line find(view pattern, view directory = ".");
		// Paths to matching files in directory

		line which(view name);
		// Paths to executables with program name

		line open(view path);
		// Preferred application for file type at path
	};

	struct shell & cmd();
}

#endif // file
