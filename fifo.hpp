#ifndef fifo_hpp
#define fifo_hpp

#include "file.hpp"
#include "str.hpp"

namespace sys::file
{
	struct fifo : descriptor
	{
		fifo(fmt::string_view name, openmode mode = in);
		~fifo();

		operator fmt::string_view() const
		{
			return path;
		}

	private:

		fmt::string path;
	};
}

#endif // file
