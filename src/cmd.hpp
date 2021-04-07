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
		using page   = string::page;
		using out    = string::out::ref;
		using in     = string::in::ref;

		vector cache;
		string last;
		int status;

		page get(in, char end = '\n', int count = 0);
		// Cache all lines in to end, return indices

		page run(init arguments);
		// Run command as sub process

		page run(span arguments);
		// Run command as sub process

		page list(view directory = ".");
		// List files in directory

		page copy(view path);
		// Lines in file given by path

		page find(view pattern, view directory = ".");
		// Paths to matching files in directory

		page which(view name);
		// Paths to executables with program name

		page open(view path);
		// Preferred application for file type at path
	};

	struct shell & cmd();
}

#endif // file
