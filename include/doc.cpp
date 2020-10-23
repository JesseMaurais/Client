#include "doc.hpp"
#include "dig.hpp"

#include <cmath>
#include <algorithm>

namespace 
{
	template <class Pointer, class Record> class impl
	{
		fwd::vector<Record> cache;
		fwd::vector<Pointer> table;

		auto next()
		// First table iterator that is signed
		{
			return std::find_if(table.begin(), table.end(), std::signbit);
		}

		auto update(std::size_t from, Pointer to)
		// Update table directory
		{
			for (auto & it : table) 
			{
				if (from == it)
				{
					it = to;
					break;
				}
			}
		}

		auto insert(Pointer idx)
		// Lowest table iterator to storage
		{
			if (auto it = next(); table.end() != it)
			{
				*it = idx;
				return it;
			}
			else
			{
				table.emplace_back(idx);
				return table.end() - 1;
			}
		}

		auto insert(Record && that)
		// Lowest table iterator to storage
		{
			auto const size = cache.size();
			cache.emplace_back(that);
			auto const idx = static_cast<Pointer>(size);
			return insert(idx);
		}

		auto remove(Pointer id)
		// Move back into place and update table
		{
			cache.at(id) = cache.back();
			cache.pop_back();
			update(cache.size(), id);
		}

		auto reduce()
		// Erase the back of the table
		{
			while (not std::empty(table) and std::signbit(table.back()))
			{
				table.pop_back();
			}
		}

	public:

		static auto & instance()
		{
			static impl self;
			return self;
		}

		auto begin(Record && that)
		{
			auto it = insert(that);
			auto idx = std::distance(table.begin(), it);
			return static_cast<Pointer>(idx);
		}

		auto& at(Pointer id)
		{
			return cache.at(table.at(id));
		}

		void end(Pointer id)
		{
			remove(id);
			reduce();
		}
	
	};
}

namespace doc
{
	template
	<
		class Record, class Pointer
	>
	Pointer object::begin(Record && that)
	{
		impl<Pointer, Record>::instance().begin(that);
	}

	template
	<
		class Record, class Pointer
	>
	object<Record, Pointer>::reference object::at(Pointer id)
	{
		impl<Pointer, Record>::instance().at(id);
	}

	template
	<
		class Record, class Pointer
	>
	void object::end(Pointer id)
	{
		impl<Pointer, Record>::instance().end(id);
	}
}
