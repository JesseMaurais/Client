#ifndef opt_hpp
#define opt_hpp "Program Options"

#include "fmt.hpp"

namespace env::opt
{
	fmt::view get(fmt::pair, fmt::view);
	bool set(fmt::pair, fmt::view);
	fmt::view get(fmt::view, fmt::view);
	bool set(fmt::view, fmt::view);

	bool get(fmt::pair, bool);
	bool set(fmt::pair, bool);
	bool get(fmt::view, bool);
	bool set(fmt::view, bool);

	long get(fmt::pair, long, int base = 10);
	bool set(fmt::pair, long, int base = 10);
	long get(fmt::view, long, int base = 10);
	bool set(fmt::view, long, int base = 10);

	float get(fmt::pair, float);
	bool set(fmt::pair, float, int digits = 6);
	float get(fmt::view, float);
	bool set(fmt::view, float, int digits = 6);

	fmt::output put(fmt::output);
	// Write options to output string

	fmt::input get(fmt::input);
	// Read options from input string
};

#endif // file
