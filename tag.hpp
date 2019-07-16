#ifndef tag_hpp
#define tag_hpp

#include "fmt.hpp"

namespace fmt
{
	template <class Char> class basic_tag : public ctype<Char>
	{
		using base = ctype<Char>;
		using typename base::mask;
		using typename base::size;

		constexpr auto null = base::null;

		pair<Char> del;
		size index = null;

		basic_tag(Char first, Char second, size pos)
		: del(first, second), index(pos)
		{ }

	public:

		basic_tag(Char first, Char second)
		: bracket(first, second)
		{ }

	protected:

		bool do_is(mask x, Char c) const override
		{
			return (x & space) ? (null == index) : base::is(x, c);
		}

		Char const* do_scan_is(mask x, Char const* it, Char const* end) const override
		{
			auto tag = *this;
			while (it != end)
			{
				Char const c = *it;

				if (c == del.first)
				{
					++tag.index;
				}
				else
				if (c == del.second)
				{
					--tag.index;
				}

				if (tag.is(x, c))
				{
					break;
				}
				else ++it;
			}
			return it;
		}

		Char const* do_scan_not(mask x, Char const* it, Char const* end) const override
		{
			auto tag = *this;
			while (it != end)
			{
				Char const c = *it;

				if (c == del.first)
				{
					--tag.index;
				}
				else
				if (c == del.second)
				{
					++tag.index;
				}

				if (tag.is(x, c))
				{
					break;
				}
				else ++it;
			}
			return it;
		}
	};

	using tag = basic_tag<char>;
	using wtag = basic_tag<wchar_t>;
}

#endif // file
