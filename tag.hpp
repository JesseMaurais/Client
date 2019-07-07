#ifndef tag_hpp
#define tag_hpp

#include "str.hpp"
#include <functional>
#include <algorithm>

namespace fmt
{
	template <class Char> struct basic_tag : basic_span_view<Char>
	{
		using base = basic_span_view<Char>;
		using string_view = basic_string_view<Char>;
		using iterator = typename base::iterator;
		using index = typename string_view::size_type;
		using reactor = std::function<bool(iterator, index)>;

		using base::base;

		bool scan(string_view u, reactor r) const
		{
			auto it = start();
			auto const z = u.size();
			for (index i = 0, j = 1; j <= z; ++j)
			{
				auto const n = j - i;
				auto const v = u.substr(i, n);
				it = std::equal_range(it.first, it.second, v);
				if (it.first == it.second)
				{
					it = start();
					i = j;
				}
				else
				if (it.first->size() == n)
				{
					if (r(it.first, i))
					{
						return true;
					}
				}
			}
			return false;
		}

	private:

		pair<iterator> start() const
		{
			return { base::begin(), base::end() };
		}
	};

	using tag = basic_tag<char>;
	using wtag = basic_tag<wchar_t>;
}

#endif // file

