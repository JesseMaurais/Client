#ifndef dev_hpp
#define dev_hpp

#include "env.hpp"

namespace env::dev
{
	inline env::span const& bin = ::env::paths;
	extern env::span const& lib;
	extern env::span const& share;
	extern env::span const& include;
	extern env::view const& lflags;
	extern env::view const& cflags;
	extern env::view const& cc;
}

#endif // file
