#ifndef shm_hpp
#define shm_hpp "Shared Memory"

#include <functional>
#include <memory>
#include "mode.hpp"

namespace env::file
{
	using memory = std::unique_ptr<void, std::function<void(void*)>>;

	memory make_shm(int, size_t = 0, off_t = 0, mode = rw, size_t* = nullptr);
}

#endif // file
