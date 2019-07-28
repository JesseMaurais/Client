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

	using mask = predicate<fmt::string_view>;

	mask copy(fmt::string&);
	mask match(fmt::string_view);
	mask insert(fmt::string_vector&);
	mask mode(sys::file::mode);

	constexpr auto stop = contra<fmt::string_view>;

	bool find(fmt::string_view dir, mask);

	inline bool find(fmt::string_view_span list, mask view)
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
