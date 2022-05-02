#ifndef tag_hpp
#define tag_hpp "Shared String Store"

#include "fmt.hpp"

namespace fmt::tag
{
	using view = fmt::string::view;
	using path = fmt::diff;
	using node = path::type;
	using pair = path::pair;

	bool got(node); // O (1)
	bool got(view); // O log n
	view get(node); // O (1)
	view get(view); // O log n
	node put(view); // O log n
	node set(view); // O log n

	string::in::ref get(string::in::ref, char = eol);
	// Read all file lines to cache

	string::out::ref put(string::out::ref, char = eol);
	// Write all cache lines to file
}

#endif // file
