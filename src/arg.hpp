#ifndef arg_hpp
#define arg_hpp "Command Line Arguments"

#include "fmt.hpp"
#include "opt.hpp"

namespace env::opt
{
	fmt::string::view application();
	fmt::string::view::span arguments();
	fmt::string::view initials();
	fmt::string::view program();
	fmt::string::view config();
	fmt::string::view cache();
	fmt::string::view rundir();

	inline auto arg(size_t argn)
	{
		auto const argv = arguments();
		return argv[argn];
	}

	constexpr auto dash = "-";
	constexpr auto dual = "--";

	struct command
	{
		unsigned int argn; // required arguments (or -1 for any number)
		fmt::string::view dash; // short name with one dash
		fmt::string::view name; // long name with dual dash
		fmt::string::view text; // descriptive text for users
	};

	using commands = fwd::span<command>;

	fmt::string::view::vector put(int argc, char** argv, commands);
	// Put command line arguments into options
	fmt::string::out::ref put(fmt::string::out::ref);
	// Write options to output string
	fmt::string::in::ref get(fmt::string::in::ref);
	// Read options from input string

	bool got(word);
	fmt::string::view get(word);
	bool set(word, fmt::string::view);
	bool put(word, fmt::string::view);

	inline auto get(word wd, fmt::string::view u)
	{
		auto v = get(wd);
		return empty(v) ? u : v;
	}

	bool got(pair);
	fmt::string::view get(pair);
	bool set(pair, fmt::string::view);
	bool put(pair, fmt::string::view);

	inline auto get(pair wd, fmt::string::view u)
	{
		auto v = get(wd);
		return empty(v) ? u : v;
	}
};

#endif // file
