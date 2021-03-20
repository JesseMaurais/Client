#ifndef meta_hpp
#define meta_hpp "Document Metadata"

#include "doc.hpp"
#include "sync.hpp"

namespace doc
{
	using namespace std;
	using namespace fwd;
	using namespace fmt;

	template <class Type> auto& instance<Type>::self()
	{
		static instance singleton;
		return singleton;
	}

	template <class Type> auto& instance<Type>::at(size_t pos)
	{
		return item.at(index.at(pos));
	}

	template <class Type> bool instance<Type>::erase(size_t pos)
	{
		auto& offset = index.at(pos);
		if (offset < 0) --offset;
		else
		{
			// move back into position
			item.at(offset) = move(item.back());
			item.pop_back();
			pos = item.size();
			// point back at offset
			for (auto& it : index)
			{
				if (pos == it)
				{
					it = offset;
					break;
				}
			}
			offset = -1;
		}
		return offset;
	}
	
	template <class Type> size_t instance<Type>::emplace(Type&& type)
	{
		// lowest free index
		auto low = index.size();
		if (gap() > 0)
		{
			for (auto count : up_to(size))
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

#endif // file
