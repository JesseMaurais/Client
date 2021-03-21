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
		auto const off = index.at(pos);
		#ifdef assert
		assert(cross.at(off) == pos);
		#endif
		return item.at(off);
	}

	template <class Type> Type* instance<Type>::find(size_t pos)
	{
		Type* ptr = nullptr;
		if (index.size() > pos)
		{
			ptrdiff_t const sz = item.size();
			if (auto const off = index.at(pos); not signbit(off) and off < sz)
			{
				#ifdef assert
				assert(cross.at(off) == pos);
				#endif
				ptr = item.data() + off;
			}
		}
		return ptr;
	}

	template <class Type> ptrdiff_t instance<Type>::free(size_t pos)
	{
		auto const off = index.at(pos);
		if (not signbit(off))
		{
			#ifdef assert
			assert(cross.at(off) == pos);
			#endif

			// move back into position
			item.at(off) = move(item.back());
			item.pop_back();
			index.at(off) = -1;
			// point back at new offset
			pos = item.size();
			if (0 < pos)
			{
				cross.at(off) = cross.at(pos);
				index.at(cross.at(pos)) = off;
			}
			cross.pop_back();

			#ifdef assert
			assert(cross.size() == item.size());
			#endif
		}
		return off;
	}
	
	template <class Type> size_t instance<Type>::make(Type&& type)
	{
		// lowest free index
		auto low = index.size();
		if (gap() > 0)
		{
			for (auto count : up_to(index.size()))
			{
				if (index.at(count) < 0)
				{
					low = count;
					break;
				}
			}
		}
		// allocate a position
		auto const pos = item.size();
		if (index.size() == low)
		{
			index.push_back(pos);
		}
		else
		{
			index.at(low) = pos;
		}
		// allocate an object
		item.emplace_back(type);
		return low;
	}
}
