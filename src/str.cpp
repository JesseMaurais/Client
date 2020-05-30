#include "str.hpp"
#include "dig.hpp"
#include "err.hpp"
#include "file.hpp"
#include "sync.hpp"

namespace
{
	sys::exclusive<fmt::string::set> cache;
	sys::exclusive<fmt::string::view::vector> store;
	sys::exclusive<std::map<fmt::string::view, env::opt::word>> table;
}

namespace fmt::str
{
	bool got(word name)
	{
		word const index = ~name;
		env::file::ssize_t const size = store.read()->size();
		return -1 < index and index < size;
	}

	bool got(view name)
	{
		auto const reader = table.read();
		auto const it = reader->find(name);
		auto const end = reader->end();
		return end != it;
	}

	view get(word name)
	{
		auto const reader = store.read();
		word const index = ~name;
		return reader->at(index);
	}

	view get(view name)
	{
		// Lookup extant
		{
			auto const reader = table.read();
			auto const it = reader->find(name);
			auto const end = reader->end();
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
			auto const reader = table.read();
			auto const end = reader->end();
			auto const it = reader->find(name);
			if (end != it)
			{
				return it->second;
			}
		}
		// Create entry
		auto writer = store.write();
		auto const size = writer->size();
		auto const id = to<word>(size);
		{
			auto [it, unique] = table.write()->emplace(name, id);
			#ifdef verify
			verify(unique);
			#endif
			writer->push_back(it->first);
		}
		return id;
	}

	word set(view name)
	{
		// Lookup extant
		{
			auto const reader = table.read();
			auto const end = reader->end();
			auto const it = reader->find(name);
			if (end != it)
			{
				return it->second;
			}
		}
		// Create entry
		auto writer = store.write();
		auto const size = writer->size();
		auto const id = to<word>(size);
		{
			// Cache the string here
			auto const p = cache.write()->emplace(name);
			verify(p.second);
			// Index a view to the string
			auto const q = table.write()->emplace(*p.first, id);
			verify(q.second);
			
			writer->push_back(q.first->first);
		}
		return id;
	}

	string::in::ref get(string::in::ref in, char end)
	{
		// Block all threads at this point
		auto wcache = cache.write();
		auto wstore = store.write();
		auto wtable = table.write();

		string line;
		while (std::getline(in, line, end))
		{
			auto const p = wcache->emplace(move(line));
			#ifdef verify
			verify(p.second);
			#endif

			auto const size = wstore->size();
			auto const id = to<word>(size);	
			wstore->emplace_back(*p.first);

			auto const q = wtable->emplace(*p.first, id);
			#ifdef verify
			verify(q.second);
			#endif
		}
		return in;
	}

	string::out::ref put(string::out::ref out, char eol)
	{
		auto const reader = cache.read();
		auto const begin = reader->begin();
		auto const end = reader->end();

		for (auto it = begin; it != end; ++it)
		{
			out << *it << eol;
		}
		return out;
	}
}

