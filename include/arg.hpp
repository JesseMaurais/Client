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

	struct cmd : fmt::layout<cmd>
	{
		long argn; // required arguments (or -1 for any number)
		view dash; // short name with one dash (-c)
		view name; // long name with dual dash (--config)
		view text; // descriptive text for user help menu
	};

	fmt::vector put(int argc, char** argv, cmd::span);
	// Put command line arguments into options
};

#endif // file
