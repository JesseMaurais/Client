#ifndef dev_hpp
#define dev_hpp

#include "env.hpp"

namespace env::dev
{
	extern env::view const& run;
	inline env::span const& bin = ::env::paths;
	extern env::span const& lib;
	extern env::span const& share;
	extern env::span const& include;
	extern env::span const& flags;
	extern env::view const& cc;
}

#endif // file
