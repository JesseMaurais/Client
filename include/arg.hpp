#ifndef arg_hpp
#define arg_hpp "Command Line Arguments"

#include "opt.hpp"

namespace env::opt
{
	view application();
	span arguments();
	view initials();
	view program();
	view config();

	bool got(view);
	view get(view);
	bool put(view, view);

	bool got(pair);
	view get(pair);
	bool put(pair, view);

	inline auto arg(size_t argn = 0)
	{
		const auto argv = arguments();
		return argv[argn];
	}

	struct command
	{
		long argn; // required arguments (or -1 for any number)
		view dash; // short name with one dash (-c)
		view name; // long name with dual dash (--config)
		view text; // descriptive text for user help menu

		using vector = fwd::vector<command>;
		using span = fwd::span<command>;
	};

	fmt::vector put(int argc, char** argv, command::span);
	// Put command line arguments into options

	fmt::output put(fmt::output);
	// Write options to output string

	fmt::input get(fmt::input);
	// Read options from input string
};

#endif // file
