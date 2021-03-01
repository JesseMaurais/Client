#ifndef dev_hpp
#define dev_hpp

#include "shell.hpp"

namespace env::dev
{
	struct dump : shell
	{
		line sym(view path);
		line dyn(view path);
	};
}

#endif // file
