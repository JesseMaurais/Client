// Do not include from another header

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
		#ifdef assert
		assert(cross.at(index.at(pos)) == pos);
		#endif

		Type* ptr = nullptr;
		if (index.size() > pos)
		{
			ptrdiff_t const sz = item.size();
			if (auto const off = index.at(pos); not signbit(off) and off < sz)
			{
				ptr = item.data() + off;
			}
		}
		return ptr;
	}

	template <class Type> size_t instance<Type>::free(size_t pos)
	{
		#ifdef assert
		assert(cross.at(index.at(pos)) == pos);
		#endif

		auto const off = index.at(pos);
		item.at(off) = move(item.back());
		cross.at(off) = cross.back();
		index.at(cross.back()) = off;
		index.at(pos) = -1;

		item.pop_back();
		cross.pop_back();
		while (index.back() < 0)
		{
			index.pop_back();
		}

		#ifdef assert
		assert(cross.size() == item.size());
		#endif

		return item.size();
	}
	
	template <class Type> size_t instance<Type>::make(Type&& type)
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
		// allocate an object
		item.emplace_back(type);
		cross.push_back(pos);
		return pos;
	}
}

