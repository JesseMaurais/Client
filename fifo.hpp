#ifndef fifo_hpp
#define fifo_hpp

#include "file.hpp"
#include "str.hpp"
#include "fs.hpp"

namespace sys::file
{
	int convert(perms); // C++ to POSIX

	struct fifo : descriptor
	{
		fifo(fmt::string_view name, perms access = all_access, openmode mode = in);
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
