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

	bool got(name);
	view get(name);
	bool put(name, view);

	bool got(pair);
	view get(pair);
	bool put(pair, view);

	inline auto arg(size_t argn = 0)
	{
		auto const argv = arguments();
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

	view::vector put(int argc, char** argv, command::span);
	// Put command line arguments into options

	fmt::string::out::ref put(fmt::string::out::ref);
	// Write options to output string

	fmt::string::in::ref get(fmt::string::in::ref);
	// Read options from input string
};

#endif // file
