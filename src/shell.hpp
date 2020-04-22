#ifndef shell_hpp
#define shell_hpp "Command Shell"

#include "fmt.hpp"

namespace env
{
	struct shell
	{
		using fmt::string;
		using string::view;
		using string::vector;
		using string::span;
		using string::in;
		using string::out;
		using string::pair;
		using string::edges;
		using string::subspan;

		vector cache;
		string line;

		subspan get(in, char end = '\n', int count = 0);
		// Cache all lines in to end, return indices

		subspan list(view directory = ".");
		// Contents of directory given by path

		subspan copy(view path);
		// Contents of file given by path

		subspan find(view pattern, view directory = ".");
		// Paths to matching files in directory

		subspan which(view name);
		// Paths to executables with program name

		subspan open(view path);
		// Preferred application
	};
}

#endif // file
