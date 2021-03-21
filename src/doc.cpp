// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "meta.hpp"
#include "err.hpp"

namespace
{
	using namespace std::literals::string_view_literals;

	struct dummy
	{
		int i = 0;
		long n = 0;
		short w = 0;
		char b = 0;
		float f = 0.0f;
		double d = 0.0;
		fmt::string s = "Hello World";

		static auto table()
		{
			return std::tuple
			{ 
				std::pair { &dummy::i, "i"sv }, 
				std::pair { &dummy::f, "f"sv } 
			};
		}

	} dumb;

}

template class doc::instance<dummy>;

#ifdef test_unit
test_unit(doc)
{
	using parent_type = fwd::offset_of<&dummy::i>::parent_type;
	static_assert(std::is_same<parent_type, dummy>::value);

	using value_type = fwd::offset_of<&dummy::i>::value_type;
	static_assert(std::is_same<value_type, int>::value);

	size_t const id = doc::access<dummy>().make({});
	assert(0 == id);
	auto ptr = doc::access<dummy>().find(id);
	assert(nullptr != ptr);

	doc::value<0>(ptr) = 42;
	doc::value<1>(ptr) = 4.2f;

	assert(42 == ptr->i);
	assert(4.2f == ptr->f);

	assert(42 == doc::value<0>(ptr));
	assert(4.2f == doc::value<1>(ptr));

	assert(doc::key<0>(ptr) == "i"sv);
	assert(doc::key<1>(ptr) == "f"sv);

	doc::access<dummy>().free(id);
}
#endif
