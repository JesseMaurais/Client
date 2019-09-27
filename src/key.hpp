#ifndef key_hpp
#define key_hpp

#include "opt.hpp"

namespace env::opt
{
	bool got(word key);
	view get(word key);
	word set(view value);
	word put(view value);
}

#endif // file
