#ifndef arg_hpp
#define arg_hpp "Command Line Options"

#include "opt.hpp"

namespace env::opt
{
	extern env::view::ref application;
	extern env::span::ref arguments;
	extern env::view::ref program;
	extern env::view::ref config;
	extern env::view::ref cache;
	extern env::view::ref rundir;

	inline auto arg(size_t argn)
	{
		span const argv = arguments;
		return argv[argn];
	}

	struct description
	{
		word argn; // required arguments (or -1 for any number)
		view dash; // short name with one dash
		view name; // long name with dual dash
		view text; // descriptive text for users
	};

	using commands = fwd::init<description>;
	vector put(int argc, char** argv, commands);

	out::ref put(out::ref);
	in::ref get(in::ref);

	string directory(view);
	string initials(view);

	bool got(view key);
	view get(view key);
	bool set(view key, view value);
	bool put(view key, view value);
	bool cut(view key);
	bool got(pair key);
	view get(pair key);
	bool set(pair key, view value);
	bool put(pair key, view value);
	bool cut(pair key);
};

#endif // file
