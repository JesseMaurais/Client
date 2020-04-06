#ifndef fpe_hpp
#define fpe_hpp "Floating Point Environment

#include "env.hpp"
#include <cfloat>
#include <cfenv>

namespace env::fp
{
	static struct : variable<int>
	{
		operator type() const final
		{
			return std::fegetround();
		}

		type operator=(type value) final
		{
			return std::fesetround(value);
		}

	} round;

	struct state : fenv_t
	{
		int get() 
		{
			return std::fegetenv(this); 
		}

		int set() const
		{ 
			return std::fesetenv(this); 
		}

		int hold() 
		{
			return std::feholdexcept(this); 
		}

		int update() const 
		{
			return std::feupdateenv(this); 
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
		int bits;

		except(int bitmask) : bits(bitmask)
		{
			(void) std::fegetexceptflag(this, bits);
		}

		~except()
		{
			return std::fesetexceptflag(this, bits);
		}
	};

	constexpr auto clear = std::feclearexcept;
	constexpr auto raise = std::feraiseexcept;
	constexpr auto check = std::fetestexcept;
}

#endif // file
