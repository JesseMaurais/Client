#ifndef meta_hpp
#define meta_hpp "Document Templates"
#else // Do not include this header from another header!
#error Included more than once. Did you include it from a header?
#endif

#include "doc.hpp"
#include "it.hpp"
#include "dig.hpp"

namespace doc
{
	using namespace std;
	using namespace fwd;
	using namespace fmt;

	template <class Type> instance<Type>& instance<Type>::self()
	{
		static instance singleton;
		return singleton;
	}

	template <class Type> const int instance<Type>::open(Type&& type)
	{
		// find lowest free index
		auto pos = index.size();
		if (gap() > 0)
		{
			for (auto count : up_to(pos))
			{
				if (index.at(count) < 0)
				{
					pos = count;
					break;
				}
			}
		}
		// allocate a position
		auto const off = item.size();
		if (index.size() == pos)
		{
			index.push_back(off);
		}
		else
		{
			index.at(pos) = off;
		}
		// allocate an item
		item.emplace_back(type);
		cross.push_back(pos);

		#ifdef assert
		assert(cross.size() == item.size());
		assert(cross.at(index.at(pos)) == pos);
		#endif

		return fmt::as_int(pos);
	}

	template <class Type> const int instance<Type>::close(const int id)
	{
		auto const pos = fmt::to_size(id);
		#ifdef assert
		assert(find(pos));
		#endif

		auto const off = index.at(pos);
		item.at(off) = move(item.back());
		cross.at(off) = cross.back();
		index.at(cross.back()) = off;
		index.at(pos) = -1;
		while (index.back() < 0)
		{
			index.pop_back();
		}
		cross.pop_back();
		item.pop_back();

		#ifdef assert
		assert(cross.size() == item.size());
		assert(item.empty() or cross.at(index.at(off)) == to_size(off));
		#endif

		auto const size = item.size();
		return fmt::to_int(size);
	}

	template <class Type> Type* instance<Type>::find(int id)
	{
		if (auto const pos = fmt::to_size(id); in_range(index, pos))
		{
			if (auto const off = index.at(pos); in_range(item, off))
			{
				#ifdef assert
				assert(pos == cross.at(off));
				#endif
				return item.data() + off;
			}
		}
		return nullptr;
	}

	template <class Type> Type& instance<Type>::at(int id)
	{
		auto const pos = fmt::to_size(id);
		#ifdef assert
		assert(cross.at(index.at(pos)) == pos);
		#endif
		return item.at(index.at(pos));
	}
}
