#ifndef opt_hpp
#define opt_hpp "Program Options"

#include "fmt.hpp"

namespace env::opt
{
	using word = long long;
	using quad = long double;
	using name = signed short;
	using pair = fwd::pair<name>;
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
	word get(name, word, int base = 10);
	bool set(name, word, int base = 10);
	word get(pair, word, int base = 10);
	bool set(pair, word, int base = 10);

	// Float
	quad get(name, quad);
	bool set(name, quad, int digits = 6);
	quad get(pair, quad);
	bool set(pair, quad, int digits = 6);
};

#endif // file
