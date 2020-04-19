#ifndef shell_hpp
#define shell_hpp "Command Shell"

#include "fmt.hpp"

namespace env
{
	struct shell
	// Command execution shell
	{
		using fmt::string;
		using string::view;
		using string::vector;
		using string::span;
		using string::in;
		using string::out;
		using string::pair;
		using string::edges;

		vector cache;
		string line;

		static constexpr auto current = ".";
		static constexpr auto parent = "..";
		static constexpr auto text = "*.txt";
		static constexpr auto infinite = 0;

		span each(pair pos) const
		// Position to span in cache
		{
			auto const data = cache.data();
			return { data + pos.first, data + pos.second };
		}

		pair get(in, char end = '\n', int count = infinite);
		// Cache all lines in to end, return indices

		pair open(view path);
		// Open preferred application

		pair list(view directory = current);
		// Contents of directory given by path

		pair copy(view path);
		// Contents of file given by path

		pair find(view pattern = text, view directory = current);
		// Paths to matching files in directory

		pair which(view name);
		// Paths to executables with program name
	};
}

#endif // file
