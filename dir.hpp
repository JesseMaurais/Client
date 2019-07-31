#ifndef dir_hpp
#define dir_hpp

#include "env.hpp"
#include "fun.hpp"
#include "file.hpp"

namespace fmt::path
{
	fmt::string_view_vector split(fmt::string_view);

	namespace impl
	{
		fmt::string join(fmt::string_view_span);
	}

	template <typename... StringView>
	inline auto join(StringView const&... args)
	{
		return impl::join({ fmt::string_view(args)... });
	}

	template <typename StringViewSpan>
	inline auto join(StringViewSpan const& list)
	{
		return impl::join(fmt::string_view_span(list));
	}
}

namespace fmt::dir
{
	fmt::string_view_vector split(fmt::string_view);

	namespace impl
	{
		fmt::string join(fmt::string_view_span);
	}

	template <typename... StringView>
	inline auto join(StringView const&... args)
	{
		return impl::join({ fmt::string_view(args)... });
	}

	template <typename StringViewSpan>
	inline auto join(StringViewSpan const& list)
	{
		return impl::join(fmt::string_view_span(list));
	}
}

namespace env::dir
{
	inline env::view const& tmp = ::env::tmpdir;
	extern env::view const& run;
	inline env::list const& bin = ::env::paths;
	extern env::list const& lib;
	extern env::list const& share;
	extern env::list const& include;

	using view = etc::predicate<fmt::string_view>;

	view copy(fmt::string&);
	view match(fmt::string_view);
	view insert(fmt::string_vector&);
	view mode(sys::file::mode);

	constexpr auto stop = etc::falsity<fmt::string_view>;

	bool find(fmt::string_view dir, view);

	inline bool find(fmt::string_view_span list, view visitor)
	{
		for (auto const dir : list)
		{
			if (find(dir, visitor))
			{
				return true;
			}
		}
		return false;
	}
}

#endif // file
