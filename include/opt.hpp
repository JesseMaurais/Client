#ifndef opt_hpp
#define opt_hpp "Program Options"

#include "fmt.hpp"

namespace env::opt
{
	using view = fmt::view;
	using pair = fmt::pair;
	using span = fmt::span;

	view get(pair, view);
	bool set(pair, view);
	view get(view, view);
	bool set(view, view);

	bool get(pair, bool);
	bool set(pair, bool);
	bool get(view, bool);
	bool set(view, bool);

	long get(pair, long, int base = 10);
	bool set(pair, long, int base = 10);
	long get(view, long, int base = 10);
	bool set(view, long, int base = 10);

	float get(pair, float);
	bool set(pair, float, int digits = 6);
	float get(view, float);
	bool set(view, float, int digits = 6);
};

#endif // file
