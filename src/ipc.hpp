#ifndef ipc_hpp
#define ipc_hpp

#include "str.hpp"
#include "file.hpp"

namespace sys::ipc
{
	file::process twin(fmt::string_view cmd);
}

#endif // file
