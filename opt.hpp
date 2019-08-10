#ifndef opt_hpp
#define opt_hpp

#include "env.hpp"
#include "str.hpp"

namespace env::opt
{
	extern fmt::string_view application;
	extern env::list const& arguments;
	extern env::view const& identity;
	extern env::view const& program;
	extern env::view const& config;
	extern env::view const& cache;

	using view = fmt::string_view;
	using pair = fmt::string_pair;

	void set(int argc, char** argv);
	view get(view key);
	bool set(view key, view value);
	view get(pair key);
	bool set(pair key, view value);
};

#endif // file
