#ifndef meta_hpp
#define meta_hpp "Document Templates"
#else // Do not include this header from another header!
#error Included more than once. Did you include it from a header?
#endif

#include "doc.hpp"
#include "sync.hpp"
#include "dig.hpp"
#include "it.hpp"

namespace doc
{
	template <class Type> struct object : Type
	{
		using Type::Type;
		node::vector sub;
	};

	template <class Type> struct array
	{
		fwd::vector<object<Type>> item;
		fwd::vector<std::ptrdiff_t> index;
		fwd::vector<std::size_t> cross;
	};

	static sys::exclusive<std::map<std::type_index, fwd::as_ptr<interface>> registry;

	static interface* find(std::type_index id)
	{
		auto data = registry.reader();
		auto it = data->find(id);
		return data->end() == it
			? nullptr : it->second;
	}

	template <class Type> instance<Type>::instance()
	{
		auto writer = registry.writer();
		[[maybe_unused]] auto [it, unique] = writer->emplace(typeid(Type), this);
		#ifdef assert
		assert(writer->end() != it);
		assert(unique);
		#endif
	}

	template <class Type> instance<Type>& instance<Type>::self()
	{
		static instance singleton;
		return singleton;
	}

	template <class Type> static sys::exclusive<array<Type>> global;

	template <class Type> int instance<Type>::emplace(Type&& type)
	{
		auto data = global<Type>.writer();

		// find lowest free index
		auto pos = writer->index.size();
		if (data->index.size() > data->item.size())
		{
			for (auto count : fwd::up_to(pos))
			{
				if (data->index.at(count) < 0)
				{
					pos = count;
					break;
				}
			}
		}
		// allocate a position
		auto const off = data->item.size();
		if (data->index.size() == pos)
		{
			data->index.push_back(off);
		}
		else
		{
			data->index.at(pos) = off;
		}
		// allocate an item
		data->item.emplace_back(std::move(type));
		data->cross.push_back(pos);

		#ifdef assert
		assert(data->cross.size() == data->item.size());
		assert(data->cross.at(data->index.at(pos)) == pos);
		#endif

		return fmt::to_int(pos);
	}

	template <class Type> void instance<Type>::erase(int id)
	{
		auto data = global<Type>.writer();

		auto const pos = fmt::to_size(id);
		#ifdef assert
		assert(writer->contains(pos));
		#endif

		auto const off = data->index.at(pos);
		data->item.at(off) = std::move(data->item.back());
		data->cross.at(off) = data->cross.back();
		data->index.at(data->cross.back()) = off;
		data->index.at(pos) = -1;
		while (data->index.back() < 0)
		{
			data->index.pop_back();
		}
		data->cross.pop_back();
		data->item.pop_back();

		#ifdef assert
		assert(data->cross.size() == data->item.size());
		assert(data->item.empty() or data->cross.at(data->index.at(off)) == fmt::to_size(off));
		assert(data->item.empty() or data->cross.at(data->index.at(off)) == fmt::to_size(off));
		#endif
	}

	template <class Type> bool instance<Type>::contains(int id)
	{
		auto reader = global<Type>.reader();

		if (auto const pos = fmt::to_size(id); fwd::in_range(reader->index, pos))
		{
			if (auto const off = reader->index.at(pos); fwd::in_range(reader->item, off))
			{
				#ifdef assert
				assert(pos == reader->cross.at(off));
				#endif
				return true;
			}
		}
		return false;
	}
}
