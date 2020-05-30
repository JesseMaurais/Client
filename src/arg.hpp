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

	string::view::vector put(int argc, char** argv, commands);
	// Put command line arguments into options
	string::out::ref put(string::out::ref);
	// Write options to output string
	string::in::ref get(string::in::ref);
	// Read options from input string

	bool got(word);
	string::view get(word);
	bool set(word, string::view);
	bool put(word, string::view);

	inline auto get(word wd, string::view u)
	{
		auto v = get(wd);
		return empty(v) ? u : v;
	}

	bool got(pair);
	string::view get(pair);
	bool set(pair, string::view);
	bool put(pair, string::view);

	inline auto get(pair wd, string::view u)
	{
		auto v = get(wd);
		return empty(v) ? u : v;
	}
};

#endif // file
