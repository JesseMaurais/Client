#ifndef meta_hpp
#define meta_hpp "Document Templates"
#else // Do not include this header from another header!
#error Included more than once. Did you include it from a header? Do not!
#endif

#include "doc.hpp"
#include "sync.hpp"
#include "algo.hpp"
#include "dig.hpp"
#include "it.hpp"

namespace doc
{
	// private

	template <class Type> struct element : Type
	{
		std::vector<int> link;
		using Type::Type;
		element(Type && base)
		: Type(base)
		{ }
	};

	template <class Type> struct store
	{
		std::vector<element<Type>> item;
		std::vector<std::ptrdiff_t> index;
		std::vector<std::size_t> cross;
	};

	template <class Type> static sys::exclusive<store<Type>> global;
	extern sys::exclusive<std::map<std::type_index, interface*>> registry;

	// public

	template <class Type> instance<Type>::instance()
	{
		registry.writer()->emplace(typeid(Type), this);
	}

	template <class Type> instance<Type>& instance<Type>::self()
	{
		static instance singleton;
		return singleton;
	}

	template <class Type> std::type_index instance<Type>::type() const
	{
		return typeid(Type);
	}

	template <class Type> std::ptrdiff_t instance<Type>::size() const
	{
		return global<Type>.reader()->item.size();
	}

	template <class Type> std::span<const int> instance<Type>::link(int n) const
	{
		return global<Type>.reader()->item.at(n).link;
	}

	template <class Type> typename instance<Type>::read_ptr instance<Type>::reader(int n) const
	{
		auto store = global<Type>.reader();
		auto ptr = store->item.data() + store->index.at(n);
		auto free = [s=global<Type>.reader()](fwd::as_ptr<const Type>){};
		return fwd::make_shared(fwd::cast_as<const Type>(ptr), free);
	}

	template <class Type> typename instance<Type>::write_ptr instance<Type>::writer(int n)
	{
		auto store = global<Type>.reader();
		auto ptr = store->item.data() + store->index.at(n);
		auto free = [s=global<Type>.reader()](fwd::as_ptr<Type>){};
		return fwd::make_shared(fwd::non_const<Type>(ptr), free);
	}

	template <class Type> int instance<Type>::emplace(Type&& type)
	{
		auto data = global<Type>.writer();

		// find lowest free index
		auto pos = data->index.size();
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

	template <class Type> void instance<Type>::destroy(int id)
	{
		#ifdef assert
		assert(contains(id));
		#endif

		auto data = global<Type>.writer();

		const auto pos = fmt::to_size(id);
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

	template <class Type> bool instance<Type>::contains(int id) const
	{
		auto reader = global<Type>.reader();

		if (auto pos = fmt::to_size(id); fmt::in_range(reader->index, pos))
		{
			if (auto off = reader->index.at(pos); fmt::in_range(reader->item, off))
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
