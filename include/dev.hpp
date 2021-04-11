#ifndef dev_hpp
#define dev_hpp "Developer Command Line"

#include "cmd.hpp"

namespace env
{
	struct dump : shell
	{
		page sym(view path);
		page dyn(view path);
	};

	dump & dev();
}

#endif // file
