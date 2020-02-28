#ifndef fifo_hpp
#define fifo_hpp

#include "pipe.hpp"
#include "env.hpp"
#include "str.hpp"

namespace env::file
{
	struct fifo : view, descriptor
	{
		fifo(fmt::string_view, mode = rd);
		~fifo();

		operator fmt::string_view() const override
		{
			return path;
		}

		bool connect();

	private:

		fmt::string path;
		int const flags;
	};
}

#endif // file
