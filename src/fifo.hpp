#ifndef fifo_hpp
#define fifo_hpp "Serial Device"

#include "pipe.hpp"
#include "fmt.hpp"

namespace env::file
{
	struct fifo : descriptor
	{
		fifo(fmt::string::view, mode = rd);
		~fifo();

		bool connect();

	private:

		fmt::string path;
		int const flags;
	};
}

#endif // file
