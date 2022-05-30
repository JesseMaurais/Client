#ifndef arg_hpp
#define arg_hpp "Command Line Arguments"

#include "opt.hpp"

namespace env::opt
{
	fmt::view application();
	fmt::span arguments();
	fmt::view initials();
	fmt::view program();
	fmt::view config();
	fmt::view catalog();

	bool got(fmt::view);
	fmt::view get(fmt::view);
	bool put(fmt::view, fmt::view);

	bool got(fmt::pair);
	fmt::view get(fmt::pair);
	bool put(fmt::pair, fmt::view);

	inline auto arg(size_t argn = 0)
	{
		const auto argv = arguments();
		return argv[argn];
	}

	struct cmd : fmt::layout<cmd>
	{
		long argn; // required arguments (or -1 for any number)
		fmt::view dash; // short name with one dash (-c)
		fmt::view name; // long name with dual dash (--config)
		fmt::view text; // descriptive text for user help menu
	};

	fmt::vector put(int argc, char** argv, cmd::span);
	// Put command line arguments into options
};

#endif // file
