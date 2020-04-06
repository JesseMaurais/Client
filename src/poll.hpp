#ifndef poll_hpp
#define poll_hpp "File Service"

#include "mode.hpp"
#include "fwd.hpp"

namespace env::file
{
	constexpr auto infinity = -1;
	fwd::span<int> poll(int timeout = infinity);
	int pull(int);
	int push(int, mode = rw);
	mode events(int);
}

