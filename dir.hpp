#ifndef dir_hpp
#define dir_hpp

#include "str.hpp"
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

namespace sys::file
{
	bool fail(fmt::string_view path, mode um = ok);
}

namespace sys::dir
{
	bool fail(fmt::string_view path, file::mode um = file::ok);
}

namespace env::dir
{
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

	fmt::string_view make(fmt::string_view path);
	bool remove(fmt::string_view path);

	struct tmp
	{
		tmp(fmt::string_view path)
		{
			stem = env::dir::make(path);
		}

		~tmp()
		{
			if (not empty(stem))
			{
				env::dir::remove(stem);
			}
		}

	private:

		fmt::string_view stem;
	};
}

#endif // file
