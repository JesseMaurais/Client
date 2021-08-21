#ifndef env_hpp
#define env_hpp "Environment Variables"
 
#include "fmt.hpp"

namespace env
{
	bool got(fmt::string::view);
	fmt::string::view get(fmt::string::view);
	bool set(fmt::string::view);
	bool put(fmt::string::view);
	bool put(fmt::string::view, fmt::string::view);
	fmt::string::view echo(fmt::string::view);
}

namespace env::var
{
	fmt::string::view::span all();
	fmt::string::view::span path();
	fmt::string::view temp();
	fmt::string::view pwd();
	fmt::string::view base();
	fmt::string::view root();
	fmt::string::view home();
	fmt::string::view user();
	fmt::string::view host();
	fmt::string::view lang();
	fmt::string::view shell();
	fmt::string::view domain();
	fmt::string::view session();
}

#endif // file
