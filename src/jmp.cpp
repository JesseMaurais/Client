// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

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

