#ifndef ios_hpp
#define ios_hpp

#include <iostream>
#include <sstream>
#include <fstream>
#include "ptr.hpp"

namespace io
{
	struct eat : unique, std::stringstream
	{
		eat(std::ostream& o = std::cerr) : out(o)
		{
			buf = out.rdbuf();
			out.rdbuf(rdbuf());
		}

		~eat()
		{
			out.rdbuf(buf);
		}

	private:

		std::ostream& out;
		std::streambuf* buf;
	};
}

#endif // file
