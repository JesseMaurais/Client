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
		auto const offset = index.at(pos);
		#ifdef assert
		assert(cross.at(offset) == pos);
		#endif
		return item.at(offset);
	}

	template <class Type> Type* instance<Type>::find(size_t pos)
	{
		Type* ptr = nullptr;
		if (index.size() > pos)
		{
			auto const offset = index.at(pos);
			if (-1 < offset and to_size(offset) < item.size())
			{
				#ifdef assert
				assert(cross.at(offset) == pos);
				#endif
				ptr = item.data() + offset;
			}
		}
		return ptr;
	}

	template <class Type> ptrdiff_t instance<Type>::remove(size_t pos)
	{
		auto& offset = index.at(pos);
		if (offset < 0) --offset;
		else
		{
			// move back into position
			item.at(offset) = move(item.back());
			item.pop_back();
			pos = item.size();
			// point back at new offset
			index.at(cross.at(pos)) = offset;
			cross.at(pos) = offset;
			offset = -1;
		}
		return offset;
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
