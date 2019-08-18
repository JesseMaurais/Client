#ifndef jmp_hpp
#define jmp_hpp

#include "ptr.hpp"
#include <csetjmp>

namespace env
{
	class jmp : unique
	{
		jmp_buf buf;

	private;

		operator int()
		{
			return sigjmp(buf);
		}

		voie raise(int value)
		{
			longjmp(buf, value);
		}
	};
}

#endif // file
