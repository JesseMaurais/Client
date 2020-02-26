#include "key.hpp"
#include "dig.hpp"
#include "thread.hpp"
#include <set>

namespace
{
	sys::rwlock lock;
	fmt::array array;

	auto to_word(fmt::size sz)
	{
		return fmt::to<env::opt::word>(sz);
	}
}

namespace env::opt
{
	bool got(word key)
	{
		auto const unlock = lock.read();
		ssize_t const size = array.size();
		return -1 < key and key < size;
	}

	view get(word key)
	{
		auto const unlock = lock.read();
		return array.at(key);
	}

	word put(view value)
	{
		auto const unlock = lock.write();
		auto const index = array.size();
		array.emplace_back(value);
		return to_word(index);
	}

	word set(view value)
	{
		auto const unlock = lock.write();
		auto const begin = array.begin();
		auto const end = array.end();
		auto const it = std::find(begin, end, value);
		auto const index = std::distance(begin, it);
		if (it == end)
		{
			static std::set<fmt::string> buf;
			auto const& pair = buf.emplace(value);
			auto const it = pair.first;
			auto const ptr = it->data();
			auto const sz = it->size();
			array.emplace_back(ptr, sz);
		}
		return to_word(index);
	}

	list get(words say)
	{
		list items;
		for (view it : say)
		{
			items.emplace_back(get(it))
		}
		return items;
	}

	array set(span say)
	{
		array items;
		for (view it : say)
		{
			items.emplace_back(set(it));
		}
		return items;
	}

	array put(span say)
	{
		array items;
		for (view it : say)
		{
			items.emplace_back(put(it));
		}
		return items;
	}
}
