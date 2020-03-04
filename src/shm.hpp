#ifndef shm_hpp
#define shm_hpp

#include <functional>
#include <memory>

#include "mode.hpp"
#include "str.hpp"

namespace env::file
{
	using memory = std::unique_ptr<void, std::function<void(void*)>>;

	memory make_shm(int fd, size_t sz = 0, off_t off = 0, mode am = rw, size_t *out = nullptr);
}

#endif // file
