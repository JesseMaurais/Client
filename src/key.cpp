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
	sys::rwlock lock;
	fmt::string::vector store;

	auto to_word(fmt::size_type sz)
	{
		return fmt::to<env::opt::word>(sz);
	}
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
		auto const index = store.size();
		store.emplace_back(value);
		return to_word(index);
	}

	word set(view value)
	{
		auto const unlock = lock.write();
		auto const begin = store.begin();
		auto const end = store.end();
		auto it = std::find_if(begin, end, [value](auto u)
		{
			return value == u;
		});

		if (it == end)
		{
			static fmt::string::set buf;
			bool unique;
			auto [it, unique] = buf.emplace(value);
			verify(unique);
			auto const ptr = it->data();
			auto const sz = it->size();
			store.emplace_back(ptr, sz);
		}

		auto const index = std::distance(begin, it);
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
