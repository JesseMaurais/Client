#include "key.hpp"
#include "dig.hpp"
#include <set>

namespace
{
	sys::rwlock lock;
	fmt::array list;

	auto to_word(auto sz)
	{
		return fmt::to<word>(sz);
	}
}

namespace env::opt
{
	bool got(word key)
	{
		auto const unlock = lock.read();
		auto const size = list.size();
		return -1 < key and key < size;
	}

	view get(word key)
	{
		auto const unlock = lock.read();
		return list.at(key);
	}

	word put(view value)
	{
		auto const unlock = lock.write();
		auto const index = list.size();
		list.emplace_back(value);
		return to_word(index);
	}

	word set(view value)
	{
		auto const unlock = lock.write();
		auto const begin = list.begin();
		auto const end = list.end();
		auto const it = std::find(begin, end, value);
		auto const index = std::distance(begin, it);
		if (it == end)
		{
			static std::set<fmt::string> buf;
			list.emplace_back(buf.emplace(value));
		}
		else
		{
			list.emplace_back(*it);
		}
		return to_word(index);
	}
}
