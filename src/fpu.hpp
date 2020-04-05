#ifndef fpu_hpp
#define fpu_hpp

#include "env.hpp"
#include <cfloat>
#include <cfenv>

namespace env::fp
{
	static struct : variable<int>
	{
		operator type() const final
		{
			return fegetround();
		}

		type operator=(type value) final
		{
			return fesetround(value);
		}

	} round;

	struct state : fenv_t
	{
		int get() 
		{
			return fegetenv(this); 
		}

		int set() const
		{ 
			return fesetenv(this); 
		}

		int hold() 
		{
			return feholdexcept(this); 
		}

		int update() const 
		{
			return feupdateenv(this); 
		}
	};

	class scope : state
	{
		scope()
		{
			(void) get();
		}

		~scope()
		{
			(void) set();
		}
	};

	class block : state
	{
		scope()
		{
			(void) hold();
		}

		~scope()
		{
			(void) update();
		}
	};

	struct except : fexcept_t
	{
		int get(int bits)
		{
			return fegetexceptflag(this, bits);
		}

		int set(int bits) const
		{
			return fesetexceptflag(this, bits);
		}
	};

	constexpr auto clear = feclearexcept;
	constexpr auto raise = feraiseexcept;
	constexpr auto check = fetestexcept;
}

#endif // file
