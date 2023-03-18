#ifndef arg_hpp
#define arg_hpp "Command Line Arguments"

#include "opt.hpp"

namespace fmt::opt
{
	constexpr pair posix { "--", "=" }, win32 { "/", ":" };

	string join(pair, pair = posix);
	pair split(view, pair = posix);
}

namespace env::opt
{
	using namespace fmt;
	// Application name
	view application();
	// Command line arguments
	span arguments();
	// Configuration file
	view initials();
	// Program image
	view program();
	view config();
	view catalog();

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
		fmt::view dash; // short name with one dash (-c)
		fmt::view name; // long name with dual dash (--config)
		fmt::view text; // descriptive text for user help menu
	};

	fmt::vector parse(int argc, char** argv, cmd::span);
	// Put command line arguments into options
};

#endif // file
