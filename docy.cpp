#include "opt.hpp"
#include "err.hpp"
#include <cstdlib>

fmt::string_view env::opt::application = "docy";

int main(int argc, char** argv)
{
	env::opt::set("-d", "@--debug");
	env::opt::set(argc, argv);
	sys::debug = env::opt::get("-d", sys::debug);
	std::cout << "debug is " << sys::debug << std::endl;
	return EXIT_SUCCESS;
}

