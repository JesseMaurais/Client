#ifndef dev_hpp
#define dev_hpp

#include "cmd.hpp"

namespace env
{
	struct dump : shell
	{
		line sym(view path);
		line dyn(view path);

	};
	
	dump & dev();
}

#endif // file
