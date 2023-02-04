#ifndef sig_hpp
#define sig_hpp "Signals & Sockets"

#include "fmt.hpp"
#include "tmp.hpp"

namespace sys::sig
{
	void set(fmt::view, fwd::event);
	fwd::event get(fmt::view);
	void raise(fmt::view);
}

#endif // file
