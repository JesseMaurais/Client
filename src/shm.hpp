#ifndef shm_hpp
#define shm_hpp "Shared Memory"

#include "fmt.hpp"
#include "ptr.hpp"
#include "mode.hpp"

namespace env::file
{
	using map_ptr = fwd::extern_ptr<void>;

	map_ptr make_map(int, size_t = 0, off_t = 0, mode = rw, size_t* = nullptr);
}

#endif // file
