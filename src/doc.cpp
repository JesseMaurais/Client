// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "doc.hpp"
#include "tab.hpp"
#include "err.hpp"
#include "meta.hpp"

namespace doc
{
	template struct instance<fwd::function>;

	sys::exclusive<std::map<std::type_index, interface*>> registry;
	interface* find(std::type_index index)
	{
		auto data = registry.reader();
		auto it = data->find(index);
		return data->end() == it
			? nullptr : it->second;
	}
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

template struct doc::instance<dumb>;

template <> fmt::view doc::name<&dumb::i> = "i";
template <> fmt::view doc::name<&dumb::f> = "f";
template <> fmt::view doc::name<&dumb::s> = "s";

test_unit(doc)
{
	using parent_type = fwd::offset_of<&dumb::i>::parent_type;
	static_assert(std::is_same<parent_type, dumb>::value);

	using value_type = fwd::offset_of<&dumb::i>::value_type;
	static_assert(std::is_same<value_type, int>::value);

	static auto& dummy = doc::instance<dumb>::self();
	assert(0 == dummy.size());
	const int id = dummy.emplace({});
	assert(1 == dummy.size());
	assert(0 == id);

	assert(dummy.contains(id));
	auto data = dummy.writer(id);
	auto ptr = data.get();
	assert(nullptr != ptr);

	assert(doc::key<0>(ptr) == "i");
	assert(doc::key<1>(ptr) == "f");
	assert(doc::key<2>(ptr) == "s");

	assert(doc::get<0>(ptr) == 0);
	assert(doc::get<1>(ptr) == 0.0f);
	assert(doc::get<2>(ptr) == "Hello World");

	assert(doc::get<0>(ptr) == ptr->i);
	assert(doc::get<1>(ptr) == ptr->f);
	assert(doc::get<2>(ptr) == ptr->s);

	doc::set<0>(ptr) = 42;
	doc::set<1>(ptr) = 4.2f;
	doc::set<2>(ptr) = "42";

	assert(doc::get<0>(ptr) == 42);
	assert(doc::get<1>(ptr) == 4.2f);
	assert(doc::get<2>(ptr) == "42");

	assert(doc::get<0>(ptr) == ptr->i);
	assert(doc::get<1>(ptr) == ptr->f);
	assert(doc::get<2>(ptr) == ptr->s);

	assert(1 == dummy.size());
	dummy.destroy(id);
	assert(0 == dummy.size());
}

#endif
