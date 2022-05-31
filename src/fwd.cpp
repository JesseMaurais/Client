// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "err.hpp"
#include "fwd.hpp"
#include "tmp.hpp"

#ifdef test_unit

test_unit(tmp)
{
	{
		bool ok;
		ok = false;
		{
			fwd::pop pop = [&]{ ok = true; };
		}
		assert(ok and "Run event at end of scope");
	}

	{
		fwd::source<int> src { 2, [](int n) { assert(2 == n); }};
		assert(src.get_deleter());
		assert(src.get() == 2);
	}
}

#endif