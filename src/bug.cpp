// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "err.hpp"

#ifdef test
test(bug)
{
	assert(true == true);
	assert(true != false);
	assert(true and not false);
	except(throw "Holy Cow!");
}
#endif

