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
			#pragma warning(push)
			#pragma warning(disable : 4611)
			return setjmp(buf); // macro, no std
			#pragma warning(pop)
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
		//#pragma warning(push)
		//#pragma warning(disable: 4646)
		type operator=(type) final;
		//#pragma warning(pop)
	};
}

#endif // file
