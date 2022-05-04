#ifndef tag_hpp
#define tag_hpp "Shared String Store"

#include "fmt.hpp"

namespace fmt::tag
{
	bool got(long); // O (1)
	bool got(view); // O log n
	view get(long); // O (1)
	view get(view); // O log n
	long put(view); // O log n
	long set(view); // O log n

	string::in::ref get(string::in::ref, char = eol);
	// Read all file lines to cache

	string::out::ref put(string::out::ref, char = eol);
	// Write all cache lines to file
}

#endif // file
