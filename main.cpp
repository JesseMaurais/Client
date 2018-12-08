#include "ipc.hpp"
#include <cstdlib>
#include <iostream>

int main(int argc, char **argv)
{
	sys::io::pstream ls { "ls" };
	std::string s;
	while (std::getline(ls, s))
	{
			std::cout << s << std::endl;
	}
	return EXIT_SUCCESS;
}
