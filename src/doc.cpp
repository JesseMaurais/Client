// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "doc.hpp"
#include "err.hpp"

#ifdef test_unit
test_unit(doc)
{
	using namespace std::literals::string_view_literals;

	struct dummy
	{
		int i = 0;
		float f = 0.0f;

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
	static_assert(std::is_same<parent_type, dummy>::value);

	using value_type = fwd::offset_of<&dummy::i>::value_type;
	static_assert(std::is_same<value_type, int>::value);

	doc::value<0>(dumb) = 42;
	doc::value<1>(dumb) = 4.2f;

	assert(42 == dumb.i);
	assert(4.2f == dumb.f);

	assert(42 == doc::value<0>(dumb));
	assert(4.2f == doc::value<1>(dumb));

	assert(doc::key<0>(dumb) == "i"sv);
	assert(doc::key<1>(dumb) == "f"sv);
}
#endif
