#ifndef win_file_hpp
#define win_file_hpp

#include "win.hpp"
#include "ops.hpp"

namespace sys::win
{
	struct file_info : BY_HANDLE_FILE_INFORMATION
	{
		file_info(HANDLE h)
		{
			if (not GetFileInformationByHandle(h, this))
			{
				sys::win::err(here);
			}
		}
	};
}

#endif // file
