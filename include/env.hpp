#ifndef env_hpp
#define env_hpp "Environment Variables"

#include "fmt.hpp"

namespace env
{
	bool got(fmt::view);
	fmt::view get(fmt::view);
	bool set(fmt::view);
	bool put(fmt::view);
	bool put(fmt::view, fmt::view);
	fmt::view cat(fmt::view);
	fmt::view echo(fmt::view);
	fmt::span vars();
	fmt::span path();
	fmt::view temp();
	fmt::view pwd();
	fmt::view base();
	fmt::view root();
	fmt::view home();
	fmt::view user();
	fmt::view host();
	fmt::view lang();
	fmt::view shell();
	fmt::view domain();
	fmt::view session();
}

#endif // file
