#ifndef shell_hpp
#define shell_hpp "Command Shell"

#include "fmt.hpp"

namespace env
{
	struct shell : fmt::struct_traits<shell>
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
		string line;

		lines get(in, char end = '\n', int count = 0);
		// Cache all lines in to end, return indices

		lines list(view directory = ".");
		// Directory given by path

		lines copy(view path);
		// Lines in file given by path

		lines find(view pattern, view directory = ".");
		// Paths to matching files in directory

		lines which(view name);
		// Paths to executables with program name

		lines open(view path);
		// Preferred application for file type at path
	};

	extern shell::ref cmd;
}

#endif // file
