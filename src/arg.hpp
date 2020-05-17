#ifndef arg_hpp
#define arg_hpp "Command Line Arguments"

#include "env.hpp"
#include "opt.hpp"

namespace env::opt
{
	extern env::view::ref application;
	extern env::span::ref arguments;
	extern env::view::ref initials;
	extern env::view::ref program;
	extern env::view::ref config;
	extern env::view::ref cache;
	extern env::view::ref rundir;

	inline auto arg(size_t argn)
	{
		fmt::string::view::span const argv = arguments;
		return argv[argn];
	}

	constexpr auto dash = "-";
	constexpr auto dual = "--";

	struct command
	{
		unsigned int argn; // required arguments (or -1 for any number)
		string::view dash; // short name with one dash
		string::view name; // long name with dual dash
		string::view text; // descriptive text for users
	};

	using commands = fwd::span<command>;

	vector put(int argc, char** argv, commands);
	// Put command line arguments into options

	bool got(word);
	view get(word);
	bool set(word, view);

	bool got(pair);
	view get(pair);
	bool set(pair, view);
};

#endif // file
