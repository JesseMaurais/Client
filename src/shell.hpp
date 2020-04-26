#ifndef shell_hpp
#define shell_hpp "Command Shell"

#include "fmt.hpp"

namespace env
{
	struct shell
	{
		using fmt::string;
		using string::vector;
		using string::span;
		using string::in;
		using string::out;
		using string::pair;
		using string::edges;
		using string::view;
		using view::line;
		using view::page;

		vector cache;
		string last;

		line get(in, char end = '\n', int count = 0);
		// Cache all lines in to end, return indices

		line list(view directory = ".");
		// Contents of directory given by path

		line copy(view path);
		// Contents of file given by path

		line find(view pattern, view directory = ".");
		// Paths to matching files in directory

		line which(view name);
		// Paths to executables with program name

		line open(view path);
		// Preferred application
	};
}

#endif // file
