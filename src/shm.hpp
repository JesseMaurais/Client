#ifndef shm_hpp
#define shm_hpp "Shared Memory"

#include "ptr.hpp"
#include "mode.hpp"

namespace env::file
{
	using shm_ptr = fwd::extern_ptr<void>;

	shm_ptr make_shm(int, size_t = 0, off_t = 0, mode = rw, size_t* = nullptr);
}

#endif // file
