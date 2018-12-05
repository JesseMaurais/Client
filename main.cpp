#include "ipc.hpp"
#include <cstdlib>
#include <iostream>

int main(int argc, char **argv)
{
	sys::io::pstream ls{"/bin/ls", "-l"};
	std::string res;
	while (ls)
	{
			ls >> res;
			std::cout << res << std::endl;
	}
	return EXIT_SUCCESS;
}
