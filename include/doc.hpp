#ifndef doc_hpp
#define doc_hpp "Document Structure"

#include "tmp.hpp"
#include "ptr.hpp"
#include "fmt.hpp"
#include <tuple>

namespace doc
{
	using path = fmt::diff;

	template <class Type> class instance : fwd::unique
	{
		instance() = default;

		fwd::vector<Type> item;
		fwd::vector<size_t> cross;
		fwd::vector<ptrdiff_t> index;

	public:

		static instance& self();
		const int open(Type&&);
		const int close(const int);
		Type* find(const int);
		Type& at(const int);

		inline auto gap() const
		{
			return index.size() - item.size();
		}
	};

	template <class Type> auto& access()
	{
		return instance<Type>::self();
	}

	enum class meta { data };

	template <class Type> constexpr auto tuple(const Type* = nullptr)
	{
		return Type::tuple(meta::data);
	}

	template <size_t Index, class Type> constexpr auto get(const Type* = nullptr)
	{
		return std::get<Index>(tuple<Type>());
	}

	template <auto Meta> fmt::string::view name {"(none)"};

	template <size_t Index, class Type> auto key(const Type* = nullptr)
	{
		return name<get<Index, Type>()>;
	}

	template <size_t Index, class Type> auto& value(const Type* that)
	{
		return that->*get<Index>(that);
	}

	template <size_t Index, class Type> auto& value(Type* that)
	{
		return that->*get<Index>(that);
	}
}

#endif // file
