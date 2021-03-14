// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "doc.hpp"
#include "err.hpp"

#ifdef test_unit
test_unit(doc)
{
	struct dummy
	{
		int i;
		float f;

		static auto table()
		{
			return std::tuple
			{ 
				std::pair { &dummy::i, "i"sv }, 
				std::pair { &dummy::f, "f"sv } 
			};
		}

	} dumb;

	using parent_type = fwd::offset_of<&dummy::i>::parent_type;
	static_assert(fwd::same<parent_type, dummy>);

	using value_type = fwd::offset_of<&dummy::i>::value_type;
	static_assert(fwd::same<value_type, int>);

	doc::value<0, doc::data>(&dumb) = 42;
	doc::value<1, doc::data>(&dumb) = 4.2f;

	assert(42 == dumb.i);
	assert(4.2f == dumb.f);
}
#endif
