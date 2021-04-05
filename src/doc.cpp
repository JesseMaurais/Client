// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "meta.hpp"
#include "msg.hpp"
#include "err.hpp"

namespace doc
{
	template class instance<message>;
}

#ifdef test_unit
namespace
{
	using namespace std::literals::string_view_literals;

	struct dumb
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
				&dumb::i, 
				&dumb::f,
				&dumb::s,
			};
		}
	};
}

template class doc::instance<dumb>;
template <> using doc::alias<&dumb::i> = doc::name<"i">;

test_unit(doc)
{
	using parent_type = fwd::offset_of<&dumb::i>::parent_type;
	static_assert(std::is_same<parent_type, dumb>::value);

	using value_type = fwd::offset_of<&dumb::i>::value_type;
	static_assert(std::is_same<value_type, int>::value);

	size_t const id = doc::access<dumb>().make({});
	assert(0 == id);
	auto ptr = doc::access<dumb>().find(id);
	assert(nullptr != ptr);

	doc::value<0>(ptr) = 42;
	doc::value<1>(ptr) = 4.2f;
	doc::value<2>(ptr) = "42";

	assert(42 == ptr->i);
	assert(4.2f == ptr->f);
	assert(ptr->s == "42");

	assert(42 == doc::value<0>(ptr));
	assert(4.2f == doc::value<1>(ptr));
	assert(doc::value<2>(ptr) == "42");

	assert(doc::key<0>(ptr) == "i"sv);
	assert(doc::key<1>(ptr) == "f"sv);
	assert(doc::key<2>(ptr) == "s"sv);

	doc::access<dumb>().free(id);
}
#endif
