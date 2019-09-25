#ifndef dir_hpp
#define dir_hpp

#include "str.hpp"
#include "fun.hpp"
#include "file.hpp"

namespace fmt::path
{
	vector split(view);
	string join(span<view>);
}

namespace fmt::dir
{
	vector split(view);
	string join(span<view>);
}

namespace env::dir
{
	using mode = ::sys::file::mode;
	using entry = predicate<fmt::view>;

	entry mask(mode);
	entry regx(fmt::view);
	entry copy(fmt::string&);
	entry push(fmt::buffer&);

	constexpr auto stop = always<fmt::view>;

	inline auto any(fmt::string& s, fmt::view u, mode m)
	{
		return mask(m) || regx(u) || copy(s) || stop;
	}

	inline auto all(fmt::buffer& b, fmt::view u, mode m)
	{
		return mask(m) || regx(u) || push(b);
	}

	bool find(fmt::view path, entry);
	inline bool find(fmt::span<fmt::view> list, entry peek)
	{
		for (auto const path : list)
		{
			if (find(path, peek))
			{
				return true;
			}
		}
		return false;
	}

	bool fail(fmt::view path, mode = ::sys::file::ok);
	fmt::view make(fmt::view path);
	bool remove(fmt::view path);

	class tmp
	{
		fmt::view stem;

	public:

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
	};
}

#endif // file
