#include "key.hpp"
#include "dig.hpp"
#include "file.hpp"
#include "thread.hpp"

namespace
{
	// Key storage indexed by word
	sys::exclusive<fmt::string::view::vector> store;
	// Common key persistent storage
	sys::exclusive<fmt::string::set> cache;
}

namespace env::opt
{
	using file::size_t;
	using file::ssize_t;

	bool got(word name)
	{
		ssize_t const size = store.read()->size();
		return -1 < name and name < size;
	}

	bool got(view name)
	{
		auto const read = cache.read();
		auto const it = read->find(name);
		auto const end = read->end();
		return end != it;
	}

	view get(word name)
	{
		auto const read = store.read();
		return read->at(name);
	}

	word get(view name)
	{
		return get(set(name));
	}

	word put(view name)
	{
		auto const write = store.write();
		auto const index = write->size();
		write->emplace_back(name);
		return fmt::to<word>(index);
	}

	word set(view name)
	{
		auto const write = store.write();
		auto index = write->size();
		{
			auto it = cache.read()->find(name);
			auto const end = cache.read()->end();
			if (end == it)
			{
				bool unique;
				tie(it, unique) = cache.write()->emplace(name);
				verify(unique);
				name = *it;
			}
		}
		write->push_back(name);
		return to<word>(index);
	}
}

