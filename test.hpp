#ifndef test_hpp
#define test_hpp

#include "dbg.hpp"

namespace test
{
	class interface
	{
	public:
		virtual void run() = 0;
	protected:
		interface(char const *name);
		virtual ~interface();
	};

	struct unit	: interface
	{
	protected:
		unit(char const *name)
		: interface(name)
		{ }
	};
}

#endif // file
