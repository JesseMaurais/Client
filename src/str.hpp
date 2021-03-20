#ifndef str_hpp
#define str_hpp "Shared String Store"

#include "fmt.hpp"
#include "opt.hpp"

namespace fmt::str
{
	using name = env::opt::name;
	using string = fmt::string;
	using view = string::view;

	bool got(name); // O (1)
	bool got(view); // O log n
	view get(name); // O (1)
	view get(view); // O log n
	name put(view); // O log n
	name set(view); // O log n

	string::in::ref get(string::in::ref, char = eol);
	// read all file lines to cache
	string::out::ref put(string::out::ref, char = eol);
	// write all cache lines to file
}

#endif // file
