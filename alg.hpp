#ifndef stl_hpp
#define stl_hpp

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
}

#endif // file
