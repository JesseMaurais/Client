#include <cstdlib>
#include <iostream>
#include "pstream.hpp"

int main(int argc, char **argv)
{
	sys::io::pstream ls { "tr", "a-z", "A-Z" };
	ls << "Hello, World!";
	ls.close(); // << sys::io::eof;
	std::string s;
	while (std::getline(ls, s))
	{
			std::cout << s << std::endl;
	}
	return EXIT_SUCCESS;
}
