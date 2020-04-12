#ifndef fifo_hpp
#define fifo_hpp "Serial Device"

#include "fd.hpp"
#include "env.hpp"

namespace env::file
{
	struct fifo : view, descriptor
	{
		fifo(string::view, mode = rd);
		~fifo();

		operator type() const final
		{
			return path;
		}

		bool connect();

	private:

		string path;
		int const flags;
	};
}

#endif // file
