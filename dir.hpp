#ifndef dir_hpp
#define dir_hpp

#include "env.hpp"
#include "file.hpp"
#include <functional>

struct dirent;

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
	struct mask : std::function<bool(dirent const*)>
	{
		using base = std::function<bool(dirent const*)>;
		using base::base;

		mask operator | (base const& that) const
		{
			return [&](dirent const* ent)
			{
				return this->operator()(ent) and that(ent);
			};
		}
	};

	inline env::view const& tmp = ::env::tmpdir;
	extern env::view const& run;
	inline env::list const& bin = ::env::paths;
	extern env::list const& lib;
	extern env::list const& share;
	extern env::list const& include;

	mask mode(sys::file::mode);
	mask regex(fmt::string_view);
	mask name(fmt::string&);

	bool find(fmt::string_view path, mask);

	inline bool find(fmt::string_view_span list, mask view)
	{
		for (auto const path : list)
		{
			if (find(path, view))
			{
				return true;
			}
		}
		return false;
	}
}

#endif // file
