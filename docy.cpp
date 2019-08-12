#include "opt.hpp"
#include "err.hpp"
#include <cstdlib>

fmt::string_view env::opt::application = "docy";

int main(int argc, char** argv)
{
	env::opt::set(argc, argv);
	{
		auto value = env::opt::get("quiet");
		if (not empty(value))
		{
			sys::debug = true;
		}
	}
	return EXIT_SUCCESS;
}

