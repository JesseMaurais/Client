#include "key.hpp"
#include "dig.hpp"
#include "err.hpp"
#include "file.hpp"
#include "thread.hpp"

namespace
{
	sys::exclusive<fmt::string::view::vector> store;
	sys::exclusive<std::map<fmt::string::view, env::opt::word>> index;
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
		auto const read = index.read();
		auto const end = read->end();
		auto const it = read->find(name);
		return end != it;
	}

	view get(word name)
	{
		auto const read = store.read();
		return read->at(name);
	}

	view get(view name)
	{
		// Lookup extant
		{
			auto const read = index.read();
			auto const end = read->end();
			auto const it = read->find(name);
			if (end != it)
			{
				return it->first;
			}
		}
		// Create entry
		return get(set(name));
	}

	word put(view name)
	{
		// Lookup extant
		{
			auto const read = index.read();
			auto const end = read->end();
			auto const it = read->find(name);
			if (end != it)
			{
				return it->second;
			}
		}
		// Create entry
		auto const write = store.write();
		auto const size = write->size();
		auto const id = to<word>(size);
		{
			auto [it, unique] = index.write()->emplace(name, id);
			verify(unique);
			write->push_back(*it);
		}
		return id;
	}

	word set(view name)
	{
		// Lookup extant
		{
			auto const read = index.read();
			auto const end = read->end();
			auto const it = read->find(name);
			if (end != it)
			{
				return it->second;
			}
		}
		// Create entry
		auto const write = store.write();
		auto const size = write->size();
		auto const id = to<word>(size);
		{
			static sys::exclusive<fmt::string::set> cache;
			auto const p = cache.write()->emplace(name);
			verify(p.second);
			auto const q = index.write()->emplace(*p.first, id);
			verify(q.second);
			write->push_back(*q.first);
		}
		return id;
	}
}

