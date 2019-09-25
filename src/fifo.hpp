#ifndef fifo_hpp
#define fifo_hpp

#include "file.hpp"
#include "str.hpp"

namespace sys::file
{
	struct fifo : descriptor
	{
		fifo(fmt::view name, mode mask = sys::file::rd);
		~fifo();

		operator fmt::view() const
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
