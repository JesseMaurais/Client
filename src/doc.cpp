// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "err.hpp"
#include "doc.hpp"
#include "ini.hpp"
#include "type.hpp"
#include "meta.hpp"

namespace doc
{
	template struct instance<fwd::event>;

	sys::exclusive<std::map<std::type_index, interface*>> registry;

	interface* find(std::type_index index)
	{
		auto data = registry.reader();
		auto it = data->find(index);
		return data->end() == it
			? nullptr : it->second;
	}

	fmt::input ini::getline(fmt::input buf, fmt::string::ref line)
	{
		while (std::getline(buf, line))
		{
			constexpr char omit = '#';
			// Read past comment
			auto u = fmt::view(line);
			const auto begin = u.begin();
			const auto end = u.end();
			const auto it = fmt::skip(begin, end);
			// Check line for content
			if (it != end and omit != *it)
			{
				// Trim off whitespace
				const auto t = u.find(omit);
				u = u.substr(0, t);
				u = fmt::trim(u);
				// Has text?
				if (not u.empty())
				{
					line = fmt::to_string(u);
					break; // done
				}
			}
		}
		return buf;
	}

	static bool header(fmt::view u)
	{
		return not u.empty() and u.front() == '[' and u.back() == ']';
	}

	fmt::input operator>>(fmt::input buf, ini::ref data)
	{
		fmt::string token;
		auto group = fmt::tag::empty;
		while (ini::getline(buf, token))
		{
			// Check for new group
			if (header(token))
			{
				token = token.substr(1, token.size() - 2);
				group = fmt::tag::emplace(token);
				continue;
			}
			// Extract key and value from pair
			auto [key, value] = fmt::to_pair(token);
			// Put entry into data table
			data.set({ group, key }, value);
		}
		return buf;
	}

	fmt::output operator<<(fmt::output buf, ini::cref data)
	{
		auto group = fmt::tag::empty;
		for (auto [key, value] : data.keys)
		{
			if (key.first != group)
			{
				group = key.first;
				buf << '[' << key.first << ']' << fmt::tag::eol;
			}
			buf << key.second << fmt::tag::assign << value << fmt::tag::eol;
		}
		return buf;
	}

	bool ini::got(fmt::pair key) const
	{
		return keys.find(key) != keys.end();
	}

	fmt::view ini::get(fmt::pair key) const
	{
		const auto it = keys.find(key);
		return keys.end() == it
			? fmt::tag::empty : it->second;
	}

	bool ini::set(fmt::pair key, fmt::view value)
	{
		const auto pos = cache.emplace(value);
		#ifdef assert
		assert(pos.second);
		#endif
		return put(key, *pos.first) and pos.second;
	}

	bool ini::put(fmt::pair key, fmt::view value)
	{
		auto it = keys.find(key);
		if (keys.end() == it)
		{
			auto pos = keys.emplace(key, value);
			#ifdef assert
			assert(pos.second);
			#endif
			it = pos.first;
			return pos.second;
		}
		else
		{
			it->second = value;
			return false;
		}
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

		static constexpr std::tuple table
		{
			&dumb::i,
			&dumb::f,
			&dumb::s,
		};
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

test_unit(ini)
{
	doc::ini init;

	// Data at runtime
	{
		constexpr auto group = "Group";
		constexpr auto key = "Key";
		constexpr auto value = "Value";
		// Cache value with set
		{
			auto s = fmt::to_string(value);
			bool unique = init.set({group, key}, s);
			assert(unique);
			s.clear();
		}
		// Check persistence
		{
			auto u = init.get({group, key});
			assert(not empty(u));
			assert(u == value);
		}
		// Reference with put
		{
			fmt::view u = value;
			bool unique = init.put({group, key}, u);
			assert(not unique);
		}
		// Check persistence
		{
			auto u = init.get({group, key});
			assert(not empty(u));
			assert(u == value);
			assert(u.data() == value);
		}
	}
}

#endif
