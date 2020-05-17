#ifndef fifo_hpp
#define fifo_hpp "Serial Device"

#include "pipe.hpp"
#include "env.hpp"

namespace env::file
{
	struct fifo : view, descriptor
	{
		fifo(string::view, mode = rd);
		~fifo();

		bool connect();

	private:

		string path;
		int const flags;
	};
}

#endif // file
