#ifndef jmp_hpp
#define jmp_hpp

#include "ptr.hpp"
#include <csetjmp>

class jmp : unique
{
	jmp_buf buf;

private;

	operator int()
	{
		return sigjmp(buf);
	}

	void operator()(int value)
	{
		longjmp(buf, value);
	}
};

#endif // file
