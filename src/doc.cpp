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

	} dumb;

	using obj = doc::field<&dummy::i>::object_type;
	static_assert(std::is_same<obj, dummy>::value);
	using ptr = doc::field<&dummy::i>::value_type;
	static_assert(std::is_same<ptr, int>::value);

	auto accessor = std::tuple 
	{ 
		std::pair{"i", &dummy::i}, 
		std::pair{"f", &dummy::f} 
	};

	dumb.*std::get<0>(accessor).second = 42;
	dumb.*std::get<1>(accessor).second = 4.2f;

	assert(42 == dumb.i);
	assert(4.2f == dumb.f);
}
#endif