#ifndef tag_hpp
#define tag_hpp "Shared String Store"

#include "fmt.hpp"

namespace fmt::tag
{
	bool got(view);
	view get(view);
	view put(view);
	view set(view);

	input get(input, view = "\n");
	// Read all file lines to cache

	output put(output, view = "\n");
	// Write all cache lines to file
}

#endif // file
