// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "str.hpp"
#include "dig.hpp"
#include "err.hpp"
#include "file.hpp"
#include "sync.hpp"

namespace
{
	class strings
	{
		sys::exclusive<fmt::string::set> cache;
		sys::exclusive<fmt::string::view::vector> store;
		sys::exclusive<std::map<fmt::string::view, env::opt::word>> table;

	public:

		bool got(env::opt::word name)
		{
			env::opt::word const index = ~name;
			env::file::ssize_t const size = store.read()->size();
			return -1 < index and index < size;
		}

		bool got(fmt::string::view name)
		{
			auto const reader = table.read();
			auto const it = reader->find(name);
			auto const end = reader->end();
			return end != it;
		}

		fmt::string::view get(env::opt::word name)
		{
			auto const reader = store.read();
			env::opt::word const index = ~name;
			return reader->at(index);
		}

		fmt::string::view get(fmt::string::view name)
		{
			assert(not empty(name));
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

		env::opt::word put(fmt::string::view name)
		{
			assert(not empty(name));
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
			auto const id = fmt::to<env::opt::word>(size);
			{
				auto [it, unique] = table.write()->emplace(name, id);
				#ifdef verify
				verify(unique);
				#endif
				writer->push_back(it->first);
			}
			return id;
		}

		env::opt::word set(fmt::string::view name)
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
			auto const id = fmt::to<env::opt::word>(size);
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

		fmt::string::in::ref get(fmt::string::in::ref in, char end)
		{
			// Block all threads at this point
			auto wcache = cache.write();
			auto wstore = store.write();
			auto wtable = table.write();

			fmt::string line;
			while (std::getline(in, line, end))
			{
				auto const p = wcache->emplace(move(line));
				#ifdef verify
				verify(p.second);
				#endif

				auto const size = wstore->size();
				auto const id = fmt::to<env::opt::word>(size);	
				wstore->emplace_back(*p.first);

				auto const q = wtable->emplace(*p.first, id);
				#ifdef verify
				verify(q.second);
				#endif
			}
			return in;
		}

		fmt::string::out::ref put(fmt::string::out::ref out, char eol)
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
	};

	auto& registry()
	{
		static strings singleton;
		return singleton;
	}
}

namespace fmt::str
{
	bool got(word name)
	{
		return registry().got(name);
	}

	bool got(view name)
	{
		return registry().got(name);
	}

	view get(word name)
	{
		return registry().get(name);
	}

	view get(view name)
	{
		return registry().get(name);
	}

	word put(view name)
	{
		return registry().put(name);
	}

	word set(view name)
	{
		return registry().set(name);
	}

	string::in::ref get(string::in::ref in, char end)
	{
		return registry().get(in, end);
	}

	string::out::ref put(string::out::ref out, char end)
	{
		return registry().put(out, end);
	}
}

