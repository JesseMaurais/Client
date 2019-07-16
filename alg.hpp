#ifndef alg_hpp
#define alg_hpp

#include <algorithm>
#include <utility>

namespace stl
{
	template <class Type> using pair = std::pair<Type, Type>;

	template <class Iterator> struct range : pair<Iterator>
	{
		using base = pair<Iterator>;
		using base::base;

		auto begin() const
		{
			return base::first;
		}

		auto end() const
		{
			return base::second;
		}
	};

	template <class Type> inline void swap(pair<Type>& p)
	{
		std::swap(p.first, p.second);
	}

	// Non-modifying sequence operations

	template <typename Container, typename Operation>
	inline bool all_of(Container&& c, Operation&& op)
	{
		return std::all_of(begin(c), end(c), op);
	}

	template <typename Container, typename Operation>
	inline bool any_of(Container&& c, Operation&& op)
	{
		return std::any_of(begin(c), end(c), op);
	}

	template <typename Container, typename Operation>
	inline bool none_of(Container&& c, Operation&& op)
	{
		return std::none_of(begin(c), end(c), op);
	}

	template <typename Container, typename Operation>
	inline void for_each(Container &&c, Operation &&op)
	{
		std::for_each(begin(c), end(c), op);
	}

	template <typename Value, typename Alloc, template <typename, typename> class Container>
	inline auto find(Container<Value, Alloc> const &c, Value const &value) -> decltype(c.end())
	{
		return std::find(begin(c), end(c), value);
	}

	template <typename Container, typename Predicate>
	inline auto find_if(Container &&c, Predicate &&p) -> decltype(c.end())
	{
		return std::find_if(begin(c), end(c), p);
	}

	template <typename Container, typename Predicate>
	inline auto find_if_not(Container &&c, Predicate &&p) -> decltype(c.end())
	{
		return std::find_if_not(begin(c), end(c), p);
	}

	// Modifying sequence operations

	template <typename Container, typename Iterator>
	inline auto copy(Container &&c, Iterator &&it) -> decltype(c.end())
	{
		return std::copy(begin(c), end(c), it);
	}

	template <typename Container, typename Operation>
	inline void transform(Container &&c, Operation &&op)
	{
		std::transform(begin(c), end(c), begin(c), op);
	}

	template <typename ContainerA, typename ContainerB, typename Operation>
	inline void transform(ContainerA &&a, ContainerB &&b, Operation &&op)
	{
		std::transform(begin(a), end(a), begin(b), op);
	}

	template <typename Container, typename Generator>
	inline void generate(Container &&c, Generator &&g)
	{
		std::generate(begin(c), end(c), g);
	}

	template <typename Container, typename Value>
	inline void replace(Container &&c, Value const &a, Value const &b)
	{
		std::replace(begin(c), end(c), a, b);
	}

	template <typename ToContainer, typename FromContainer>
	inline void append(ToContainer& to, FromContainer const& from)
	{
		to.reserve(to.size() + from.size());
		to.insert(end(to), begin(from), end(from));
	}

	template <typename Container, typename Predicate>
	inline void erase_if(Container&& c, Predicate&& p)
	{
		c.erase(std::remove_if(c.begin(), c.end(), p), c.end());
	}
	
	template <typename Container, typename Compare>
	inline void sort(Container &&c, Compare &&cmp)
	{
		std::sort(begin(c), end(c), cmp);
	}
}

#endif // file
