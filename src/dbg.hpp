#ifndef bug_hpp
#define bug_hpp "Debug Tests"

#include "err.hpp"

// throw just in case condition did not
#define except(expression)\
	try { assert(not(expression)); } catch(...) { }

namespace test
{
	enum class unit
	{
		fmt, ini, env, usr, dig, opt, arg, dll, shell, desktop
	};

	template <unit Type> struct entity
	{
		static void run() noexcept;
	};

	template 
	<
		unit... Types
	> 
	auto& run(int argc, char** argv, auto capture)
	{
		thread_local int results[sizeof...(Types)];
		((*results++ = entity<Types>(argc, argv)...);
		return results;
	}
}

#endif // file
