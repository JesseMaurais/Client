#ifndef tag_hpp
#define tag_hpp

#include <functional>
#include <algorithm>
#include "fmt.hpp"

namespace fmt
{
	template <class Char, template <class, template<class> class> class Type = ctype>
	class basic_tagger : Type<Char, std::ctype>
	{
		using base = Type<Char, std::ctype>;
		using size = typename base::size;
		using iterator = typename base::iterator;
		using string_view = typename base::string_view;
	
	public:

		using reactor = std::function<void(iterator, size)>;
		static void tag(iterator begin, iterator end, string_view u, reactor f)
		/// Call $f for any in sorted range $[begin, end) that is found in $u
		{
			auto it = begin;
			auto const z = u.size();
			for (size i = 0, j = 1; j <= z; ++j)
			{
				auto const n = j - i;
				auto const v = u.substr(i, n);

				it = std::lower_bound(it, end, v);
				if (end == it)
				{
					i = j;
					it = begin;
					continue;
				}

				if (it->size() == n)
				{
					f(it, i);
				}

				it = std::upper_bound(it, end, v);
				if (end == it or it->compare(0, n, v))
				{
					i = j;
					it = begin;
				}
			}
		}
	};

	using tagger = basic_tagger<char>;
	using wtagger = basic_tagger<wchar_t>;

	inline void tag(string_view_span t, string_view u, tagger::reactor r)
	{
		tagger::tag(begin(t), end(t), u, r);
	}
}

#endif // file
