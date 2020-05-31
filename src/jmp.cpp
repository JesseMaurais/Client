#include "jmp.hpp"
#ifndef _WIN32
#include "uni/jmp.hpp"
using jmpbuf = sys::uni::jmp;
#else
using jmpbuf = sys::jmp;
#endif

namespace env
{
	thread_local jmpbuf jumper;

	jmp::operator type() const
	{
		return type(jumper);
	}

	int jmp::operator=(type value)
	{
		jumper = value;
	}
}

