#ifndef shell_hpp
#define shell_hpp "Command Shell"

#include "fmt.hpp"

namespace env
{
	struct shell
	{
		using string = fmt::string;
		using view   = string::view;
		using vector = view::vector;
		using span   = view::span;
		using graph  = view::graph;
		using line   = view::line;
		using out    = view::out::ref;
		using in     = view::in::ref;

		vector cache;
		string last;

		line get(in, char end = '\n', int count = 0);
		// Cache all lines in to end, return indices

		line list(view directory = ".");
		// Directory given by path

		line copy(view path);
		// Lines in file given by path

		line find(view pattern, view directory = ".");
		// Paths to matching files in directory

		line which(view name);
		// Paths to executables with program name

		line open(view path);
		// Preferred application for file type at path
	};

	extern const shell& cmd;
}

#endif // file
