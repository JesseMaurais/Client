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
		sys::exclusive<std::map<fmt::string::view, env::opt::name>> table;

	public:

		bool got(env::opt::name key)
		{
			env::opt::name const index = ~key;
			std::ptrdiff_t const size = store.read()->size();
			return -1 < index and index < size;
		}

		bool got(fmt::string::view name)
		{
			auto const reader = table.read();
			auto const it = reader->find(name);
			auto const end = reader->end();
			return end != it;
		}

		fmt::string::view get(env::opt::name key)
		{
			auto const reader = store.read();
			auto const index = fmt::to_size(~key);
			assert(got(key) and "String is not stored");
			return reader->at(index);
		}

		fmt::string::view get(fmt::string::view key)
		{
			assert(not empty(key));
			// Lookup
			{
				auto const reader = table.read();
				auto const it = reader->find(key);
				auto const end = reader->end();
				if (end != it)
				{
					return it->first;
				}
			}
			// Create
			return get(set(key));
		}

		env::opt::name put(fmt::string::view key)
		{
			assert(not empty(key));
			// Lookup
			{
				auto const reader = table.read();
				auto const end = reader->end();
				auto const it = reader->find(key);
				if (end != it)
				{
					return ~it->second;
				}
			}
			// Create
			auto writer = store.write();
			auto const size = writer->size();
			auto const id = fmt::to<env::opt::name>(size);
			{
				auto [it, unique] = table.write()->emplace(key, id);
				assert(unique);
				writer->push_back(it->first);
			}
			return ~id;
		}

		env::opt::name set(fmt::string::view key)
		{
			// Lookup
			{
				auto const reader = table.read();
				auto const end = reader->end();
				auto const it = reader->find(key);
				if (end != it)
				{
					return ~it->second;
				}
			}
			// Create
			auto writer = store.write();
			auto const size = writer->size();
			auto const id = fmt::to<env::opt::name>(size);
			{
				// Cache the string here
				auto const p = cache.write()->emplace(key);
				verify(p.second);
				// Index a view to the string
				auto const q = table.write()->emplace(*p.first, id);
				verify(q.second);
				
				writer->push_back(q.first->first);
			}
			return ~id;
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
				assert(p.second);

				auto const size = wstore->size();
				auto const id = fmt::to<env::opt::name>(size);	
				wstore->emplace_back(*p.first);

				auto const q = wtable->emplace(*p.first, id);
				assert(q.second);
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
	bool got(name n)
	{
		return registry().got(n);
	}

	bool got(view n)
	{
		return registry().got(n);
	}

	view get(name n)
	{
		return registry().get(n);
	}

	view get(view n)
	{
		return registry().get(n);
	}

	name put(view n)
	{
		return registry().put(n);
	}

	name set(view n)
	{
		return registry().set(n);
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

