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

	void init(int argc, char** argv);
	fmt::string_view get(fmt::string_view);
	fmt::string_view get(fmt::string_view_pair);
	void put(fmt::string_view_pair, fmt::string_view);
};

#endif // file
