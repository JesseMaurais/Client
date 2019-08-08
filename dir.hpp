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

	using entry = predicate<fmt::string_view>;
	constexpr auto stop = falsity<fmt::string_view>;

	entry copy(fmt::string&);
	entry match(fmt::string_view);
	entry insert(fmt::string_vector&);
	entry mode(sys::file::mode);

	extern bool find(fmt::string_view dir, entry view);
	inline bool find(fmt::string_view_span list, entry view)
	{
		for (auto const dir : list)
		{
			if (find(dir, view))
			{
				return true;
			}
		}
		return false;
	}
}

#endif // file
