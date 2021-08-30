// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "doc.hpp"
#include "meta.hpp"
#include "ptr.hpp"
#include "msg.hpp"
#include "err.hpp"

namespace doc
{
	template class instance<function>;
}

#ifdef test_unit
namespace
{
	struct dumb
	{
		int i = 0;
		char b = 0;
		short w = 0;
		long n = 0;
		float f = 0.0f;
		double d = 0.0;
		fmt::string s = "Hello World";

		static constexpr auto table()
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
template <> fmt::string::view doc::name<&dumb::i> = "i";
template <> fmt::string::view doc::name<&dumb::f> = "f";
template <> fmt::string::view doc::name<&dumb::s> = "s";

test_unit(doc)
{
	using parent_type = fwd::offset_of<&dumb::i>::parent_type;
	static_assert(std::is_same<parent_type, dumb>::value);

	using value_type = fwd::offset_of<&dumb::i>::value_type;
	static_assert(std::is_same<value_type, int>::value);

	dumb dumber;
	const int id = doc::access<dumb>().open(dumber);
	assert(0 == id);
	const auto ptr = doc::access<dumb>().find(id);
	assert(nullptr != ptr);

	doc::set<0>(ptr) = 42;
	doc::set<1>(ptr) = 4.2f;
	doc::set<2>(ptr) = "42";

	assert(42 == ptr->i);
	assert(4.2f == ptr->f);
	assert(ptr->s == "42");

	assert(42 == doc::get<0>(ptr));
	assert(4.2f == doc::get<1>(ptr));
	assert(doc::get<2>(ptr) == "42");

	assert(doc::key<0>(ptr) == "i");
	assert(doc::key<1>(ptr) == "f");
	assert(doc::key<2>(ptr) == "s");

	doc::access<dumb>().close(id);
}
#endif
