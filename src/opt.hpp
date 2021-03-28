#ifndef opt_hpp
#define opt_hpp "Program Options"

#include "fmt.hpp"

namespace env::opt
{
	using name = fmt::diff::type;
	using pair = fmt::diff::pair;
	using view = fmt::string::view;
	using span = view::span;

	// String
	view get(name, view);
	bool set(name, view);
	view get(pair, view);
	bool set(pair, view);

	// Boolean
	bool get(name, bool);
	bool set(name, bool);
	bool get(pair, bool);
	bool set(pair, bool);

	// Integer
	long get(name, long, int base = 10);
	bool set(name, long, int base = 10);
	long get(pair, long, int base = 10);
	bool set(pair, long, int base = 10);

	// Float
	float get(name, float);
	bool set(name, float, int digits = 6);
	float get(pair, float);
	bool set(pair, float, int digits = 6);
};

#endif // file
