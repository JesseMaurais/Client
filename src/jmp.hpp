#ifndef jmp_hpp
#define jmp_hpp "Stack Jumper"

#include "env.hpp"
#include <csetjmp>
#include <csignal>

namespace sys
{
	struct jmp : fwd::variable<int>
	// Signal discarding jump
	{
		mutable jmp_buf buf;

		operator type() const final
		// Jump value or else 0
		{
			return setjmp(buf); // macro, no std
		}

		[[noreturn]] type operator=(type value) final
		// Jump without unwinding the stack
		{
			std::longjmp(buf, value);
		}
	};
}

namespace env
{
	struct jmp : fwd::variable<int>
	// Signal preserving jump
	{
		operator type() const final;
		[[noreturn]]
		type operator=(type) final;
	};
}

namespace env::sig
{
	struct jmp : constant<int>
	{
		jmp(int);
	};
}

#endif // file
