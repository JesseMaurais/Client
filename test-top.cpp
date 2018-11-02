#include "top.hpp"
#include "test.hpp"

namespace
{
	struct toplogy : test::unit
	{
		toplogy()
		: unit("topology")
		{ }

		void run() override
		{
			top::closed_interval<double> closed{0, 1};
		}

	} impl_topology;
}

