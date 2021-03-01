#ifndef uni_jmp_hpp
#define uni_jmp_hpp

#include "env.hpp"
#include <setjmp.h>

namespace sys::uni
{
	struct jmp : fwd::variable<int>
	{
		mutable sigjmp_buf buf;
		int save;

		jmp(int sigs = 0) : save(sigs)
		{ }

		operator type() const final
		{
			return sigsetjmp(buf, save);
		}

		[[noreturn]] type operator=(type value) final
		{
			siglongjmp(buf, value);
		}
	};
}

#endif // file
