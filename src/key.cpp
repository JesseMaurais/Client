#include "key.hpp"
#include "dig.hpp"
#include "file.hpp"
#ifdef _WIN32
#include "win/sync.hpp"
#else
#include "uni/pthread.hpp"
#endif
#include <set>

namespace
{
	auto to_word(fmt::size_type sz)
	{
		return fmt::to<env::opt::word>(sz);
	}

	fmt::string::view::vector store;
	sys::rwlock lock;
}

namespace env::opt
{
	using file::size_t;
	using file::ssize_t;

	bool got(word key)
	{
		auto const unlock = lock.read();
		ssize_t const size = store.size();
		return -1 < key and key < size;
	}

	view get(word key)
	{
		auto const unlock = lock.read();
		return store.at(key);
	}

	word put(view value)
	{
		auto const unlock = lock.write();
		auto const begin = store.begin();
		auto const end = store.end();
		auto const it = std::find_if(begin, end, std::bind(std::equal_to, value));
		auto const index = std::distance(begin, it);
		if (it == end)
		{
			store.emplace_back(value);
		}
		return to_word(index);
	}

	word set(view value)
	{
		auto const unlock = lock.write();
		auto const begin = store.begin();
		auto const end = store.end();
		auto const it = std::find_if(begin, end, std::bind(std::equal_to, value));
		auto const index = std::distance(begin, it);
		if (it == end)
		{
			static fmt::string::set buf;
			auto const [that, unique] = buf.emplace(value);
			verify(unique);
			auto const ptr = that->data();
			auto const sz = that->size();
			store.emplace_back(ptr, sz);
		}
		return to_word(index);
	}

	view::vector get(view::span say)
	{
		view::vector items;
		for (auto it : say)
		{
			items.emplace_back(get(it));
		}
		return items;
	}

	vector set(span say)
	{
		vector items;
		for (view it : say)
		{
			items.emplace_back(set(it));
		}
		return items;
	}

	vector put(span say)
	{
		vector items;
		for (view it : say)
		{
			items.emplace_back(put(it));
		}
		return items;
	}
}
