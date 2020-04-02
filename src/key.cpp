#include "key.hpp"
#include "dig.hpp"
#include "file.hpp"
#include "thread.hpp"
#include <set>

namespace
{
	sys::exclusive<fmt::string::view::vector> lock;
}

namespace env::opt
{
	using file::size_t;
	using file::ssize_t;

	bool got(word key)
	{
		ssize_t const size = lock.read()->size();
		return -1 < key and key < size;
	}

	bool got(view value)
	{
		auto const store = lock.read();
		auto const begin = store->begin();
		auto const end = store->end();
		auto const it = std::find(begin, it, value);
		return it != end;
	}

	view get(word key)
	{
		return lock.read()->at(key);
	}

	word get(view value)
	{
		auto const store = lock.read();
		auto const begin = store->begin();
		auto const end = store->end();
		auto const it = std::find(begin, it, value);
		auto const index = std::distance(begin, it);
		return fmt::to<word>(index);
	}

	word put(view value)
	{
		auto const store = lock.write();
		auto const begin = store->begin();
		auto const end = store->end();
		auto const it = std::find(begin, end, value);
		auto const index = std::distance(begin, it);
		if (it == end)
		{
			store->emplace_back(value);
		}
		return fmt::to<word>(index);
	}

	word set(view value)
	{
		auto const store = lock.write();
		auto const begin = store->begin();
		auto const end = store->end();
		auto const it = std::find(begin, end, value);
		auto const index = std::distance(begin, it);
		if (it == end)
		{
			static fmt::string::set buf;
			auto const [that, unique] = buf.emplace(value);
			verify(unique);
			auto const ptr = that->data();
			auto const sz = that->size();
			store->emplace_back(ptr, sz);
		}
		return fmt::to<word>(index);
	}
}

