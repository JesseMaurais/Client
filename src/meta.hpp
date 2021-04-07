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

	template <class Type> Type& instance<Type>::at(size_t pos)
	{
		#ifdef assert
		assert(cross.at(index.at(pos)) == pos);
		#endif

		return item.at(index.at(pos));
	}

	template <class Type> Type* instance<Type>::find(size_t pos)
	{
		if (in_range(index, pos))
		{
			if (auto off = index.at(pos); in_range(item, off))
			{
				#ifdef assert
				assert(cross.at(index.at(pos)) == pos);
				#endif

				return item.data() + off;
			}
		}
		return nullptr;
	}

	template <class Type> size_t instance<Type>::free(size_t pos)
	{
		#ifdef assert
		assert(find(pos));
		assert(cross.at(index.at(pos)) == pos);
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

		return item.size();
	}

	template <class Type> size_t instance<Type>::emplace(Type&& type)
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

		return pos;
	}
}
