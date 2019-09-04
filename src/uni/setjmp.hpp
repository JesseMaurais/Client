#ifndef uni_jmp_hpp
#define uni_jmp_hpp

#include "ptr.hpp"

namespace sys::uni
{
	extern "C"
	{
		#include <setjmp.h>
	}

	class jmp : unique
	{
		sigjmp_buf buf;

	private:

		int operator()(int store)
		{
			return sigsetjmp(buf, store);
		}

		void raise(int value)
		{
			siglongjmp(buf, value);
		}
	};
}

#endif // file
