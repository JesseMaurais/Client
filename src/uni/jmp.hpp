#ifndef uni_jmp_hpp
#define uni_jmp_hpp

#include "env.hpp"
#include <setjmp.h>

namespace sys::uni
{
	struct jmp : property<int>
	{
		sigjmp_buf buf;
		int save = 0;

		operator type() const final
		{
			return sigsetjmp(buf, save);
		}

		[[noreturn]] type operator=(type value) final
		{
			siglongjmp(buf);
			return -1;
		}
	};
}

#endif // file
