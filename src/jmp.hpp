#ifndef jmp_hpp
#define jmp_hpp

#include "env.hpp"
#include <csetjmp>
#include <csignal>

namespace sys
{
	struct jmp : property<int>
	{
		jmp_buf buf;

		operator type() const final
		{
			return std::setjmp(buf);
		}

		[[noreturn]] type operator=(type value) final
		{
			std::longjmp(buf);
			return -1;
		}
	}
}

namespace env
{
	struct jmp : variable<int>
	{
		operator type() const final;
		[[noreturn]]
		type operator=(type) final;
	}
}

#endif // file
