#include "str.hpp"
#include "dig.hpp"
#include "err.hpp"
#include "file.hpp"
#include "thread.hpp"

namespace
{
	sys::exclusive<fmt::string::set> cache;
	sys::exclusive<fmt::string::view::vector> store;
	sys::exclusive<std::map<fmt::string::view, env::opt::word>> index;
}

namespace fmt::str
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
			// Cache the string here
			auto const p = cache.write()->emplace(name);
			verify(p.second);
			// Index a view to the string
			auto const q = index.write()->emplace(*p.first, id);
			verify(q.second);
			
			write->push_back(*q.first);
		}
		return id;
	}

	string::in::ref get(string::in::ref in)
	{
		// Block all threads at this point
		auto const wcache = cache.write();
		auto const wstore = store.write();
		auto const windex = index.write();

		string line;
		while (std::getline(in, line))
		{
			auto const p = wcache->emplace(move(line));
			verify(p.second);

			auto const size = wstore->size();
			auto const id = to<word>(size);
	
			auto const q = wstore->emplace(*p.first, id);
			verify(q.second);

			windex->emplace_back(*q.first);
		}
		return in;
	}

	string::out::ref put(string::out::ref out)
	{
		auto const read = cache.read();
		auto const begin = read->begin();
		auto const end = read->end();

		for (auto it = begin; it != end; ++it)
		{
			out << it->first << eol;
		}
		return out;
	}
}

