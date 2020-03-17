#ifndef ipc_hpp
#define ipc_hpp

#include "mode.hpp"
#include <vector>

namespace env::file
{
	constexpr auto infinity = -1;

	std::vector<int> poll(int timeout = infinity);
	int pull(int);
	int push(int, mode = rw);
	mode events(int);
}

