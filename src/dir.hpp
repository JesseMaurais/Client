#ifndef dir_hpp
#define dir_hpp

#include "str.hpp"
#include "tmp.hpp"
#include "file.hpp"

namespace fmt::path
{
	array split(view);
	string join(span<view>);
}

namespace fmt::dir
{
	array split(view);
	string join(span<view>);
}

namespace env::dir
{
	namespace file = ::sys::file;
	using mode = file::mode;
	using xpath = fmt::view;
	using fmt::string;
	using fmt::array;
	using fmt::table;
	using entry = predicate<xpath>; // conditional
	using pass = none<xpath>; // unconditional

	entry mask(mode);
	entry regx(xpath);
	pass to(string &);
	pass to(table &);

	constexpr auto stop = always<fmt::view>;
	constexpr auto skip = never<fmt::view>;

	static auto // sys::file
		ex = mask(file::ex),
		wr = mask(file::wr),
		rd = mask(file::rd),
		ok = mask(file::ok),
		xu = mask(file::xu),
		sz = mask(file::sz),
		rw = mask(file::rw),
		wo = mask(file::wo),
		rwx = mask(file::rwx),
		oz = mask(file::oz),
		ox = mask(file::ox),
		app = mask(file::app),
		txt = mask(file::txt),
		bin = mask(file::bin);

	inline auto all(fmt::view u, mode m = file::ok, entry e = skip)
	{
		return mask(m) || regx(u) || e;
	}

	inline auto any(fmt::view u, mode m = file::ok, entry e = stop)
	{
		return all(u, m, e);
	}

	inline auto an

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

	bool fail(fmt::view path, mode = file::ok);
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
