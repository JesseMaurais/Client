#ifndef dir_hpp
#define dir_hpp

#include "str.hpp"
#include "fun.hpp"
#include "file.hpp"

namespace fmt::path
{
	std::vector<view> split(view);

	namespace impl
	{
		string join(span);
	}

	template <typename... StringView>
	inline auto join(StringView const&... args)
	{
		return impl::join({ view(args)... });
	}

	template <typename StringViewSpan>
	inline auto join(StringViewSpan const& list)
	{
		return impl::join(span(list));
	}
}

namespace fmt::dir
{
	std::vector<view> split(view);

	namespace impl
	{
		string join(span);
	}

	template <typename... StringView>
	inline auto join(StringView const&... args)
	{
		return impl::join({ view(args)... });
	}

	template <typename StringViewSpan>
	inline auto join(StringViewSpan const& list)
	{
		return impl::join(span(list));
	}
}

namespace sys::path
{
	bool fail(fmt::view path, file::mode = file::ok);
}

namespace sys::dir
{
	bool fail(fmt::view path, file::mode = file::ok);
}

namespace env::dir
{
	using entry = predicate<fmt::view>;
	constexpr auto stop = falsity<fmt::view>;

	entry copy(fmt::string&);
	entry match(fmt::view);
	entry insert(std::vector<fmt::string>&);
	entry mode(::sys::file::mode);

	extern bool find(fmt::view dir, entry view);
	inline bool find(fmt::span list, entry view)
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

	fmt::view make(fmt::view path);
	bool remove(fmt::view path);

	struct tmp
	{
		tmp(fmt::view path)
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

		fmt::view stem;
	};
}

#endif // file
