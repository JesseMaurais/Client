#include "opt.hpp"
#include "err.hpp"
#include <cstdlib>

env::opt::view const env::opt::application = "docy";

int main(int argc, char** argv)
{
	env::opt::set(argc, argv);
	{
		env::opt::commands const cmd
		{
			{ 1, "d", "debug", "Enable/disable debug messages" }
		};

		auto const extra = env::opt::parse(cmd);
		if (not empty(extra))
		{
			std::cout << "Unknown option: " << extra.front() << fmt::eol;
		}
	}
	sys::debug = env::opt::get("debug", sys::debug);
	std::cout << "debug is " << sys::debug << std::endl;

	return EXIT_SUCCESS;
}

